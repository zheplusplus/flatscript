#include <iostream>
#include <set>
#include <map>
#include <functional>
#include <util/string.h>
#include <util/pointer.h>

#include "env.h"
#include "globals.h"

#ifndef EOF
#define EOF (-1)
#endif

static std::set<std::string> const pre_defined({
    "console", "eval", "setTimeout", "setInterval", "clearTimeout", "parseInt",
    "parseFloat", "isFinite", "isNaN", "Number", "Array", "Object", "Function",
    "JSON", "RegExp", "Math", "String", "Date", "NaN", "undefined", "Infinity",
    "null", "escape", "unescape", "encodeURI", "decodeURI", "encodeURIComponent",
    "decodeURIComponent",
});

namespace {

    struct Parser;

    struct Option {
        typedef std::function<std::string(bool, std::vector<std::string>)> OptCall;

        explicit Option(Parser& parser)
            : Option(parser, {})
        {}

        Option(Parser& p, std::vector<std::string> f)
            : Option(p, std::move(f), nullptr)
        {}

        Option(Option const& rhs) = delete;
        Option(Parser& p, std::vector<std::string> f, OptCall call);
        ~Option();

        Option* set_help(std::string h)
        {
            this->help = std::move(h);
            return this;
        }

        Option* set_default(std::string d)
        {
            this->default_val = std::move(d);
            return this;
        }

        Option* set_arg_count(int c)
        {
            this->arg_count = c;
            return this;
        }

        std::string const& prime_flag() const
        {
            return *this->flags.begin();
        }

        Option& set_flags(std::vector<std::string> f);
        Option& add_flags(std::string f);
        int parse(int argc, char* argv[]);

        std::string done()
        {
            return this->opt_call(this->opt_set, std::move(this->parse_args));
        }

        void reset_result()
        {
            this->parse_args.clear();
            this->opt_set = false;
        }

        Parser& parser;
        std::string help;
        std::string default_val;
        std::vector<std::string> flags;
        OptCall opt_call;
        int arg_count;
        bool opt_set;

        std::vector<std::string> parse_args;
    };

    struct Parser {
        void add_option(util::sptr<Option> opt)
        {
            this->options.push_back(std::move(opt));
        }

        util::sref<Option> new_option()
        {
            this->add_option(util::mkptr(new Option(*this)));
            return *this->options.back();
        }

        util::sref<Option> new_option(std::vector<std::string> flags)
        {
            this->add_option(util::mkptr(new Option(*this, std::move(flags))));
            return *this->options.back();
        }

        util::sref<Option> new_option(std::vector<std::string> flags, Option::OptCall call)
        {
            this->add_option(util::mkptr(new Option(*this, std::move(flags), std::move(call))));
            return *this->options.back();
        }

        void remove_flags(std::vector<std::string> const& f)
        {
            for (std::string const& s: f) {
                opt_map.erase(s);
            }
        }

        void add_flags(std::vector<std::string> const& f, Option& opt)
        {
            for (std::string const& s: f) {
                opt_map.insert(std::make_pair(s, util::mkref(opt)));
            }
        }

        std::string usage() const
        {
            std::string::size_type max_info_len = 0;
            std::vector<std::string> flags_info;
            for (auto const& opt: this->options) {
                std::string fl(util::join(", ", opt->flags));
                max_info_len = std::max(max_info_len, fl.size());
                flags_info.push_back(std::move(fl));
            }
            std::string u("Flatscript:" + flats::Globals::g.version + "\nUsage:\n");
            for (decltype(flags_info.size()) i = 0; i < flags_info.size(); ++i) {
                u += "  ";
                u += flags_info[i];
                u += std::string(max_info_len - flags_info[i].size() + 4, ' ');
                u += this->options[i]->help;
                u += '\n';
            }
            return std::move(u);
        }

        void parse(int argc, char* argv[])
        {
            for (int i = 0; i < argc;) {
                char* arg = argv[i];
                auto find = this->opt_map.find(arg);
                if (find == this->opt_map.end()) {
                    this->parse_error = std::string("Unexpected option ") + arg;
                    return;
                }
                i += 1;
                i += find->second->parse(argc - i, argv + i);
            }
        }

        bool error() const
        {
            return !this->parse_error.empty();
        }

        bool print_error_usage() const
        {
            std::cerr << "Error: " << this->parse_error << std::endl << std::endl;
            std::cerr << this->usage() << std::endl;
            return false;
        }

        bool parsing_done()
        {
            if (this->error()) {
                return this->print_error_usage();
            }
            for (auto& o: this->options) {
                this->parse_error = o->done();
                if (!this->parse_error.empty()) {
                    return this->print_error_usage();
                }
            }
            return true;
        }

        void reset_result()
        {
            this->parse_error.clear();
            for (auto& o: this->options) {
                o->reset_result();
            }
        }

        std::map<std::string, util::sref<Option>> opt_map;
        std::vector<util::sptr<Option>> options;

        std::string parse_error;
    };

    Option::Option(Parser& p, std::vector<std::string> f, OptCall call)
        : parser(p)
        , opt_call(std::move(call))
        , arg_count(1)
        , opt_set(false)
    {
        this->set_flags(std::move(f));
    }

    Option::~Option()
    {
        this->parser.remove_flags(this->flags);
    }

    Option& Option::set_flags(std::vector<std::string> f)
    {
        this->parser.remove_flags(this->flags);
        this->flags = std::move(f);
        this->parser.add_flags(this->flags, *this);
        return *this;
    }

    Option& Option::add_flags(std::string f)
    {
        this->flags.push_back(f);
        this->parser.add_flags({f}, *this);
        return *this;
    }

    int Option::parse(int argc, char* argv[])
    {
        if (argc < this->arg_count) {
            this->parser.parse_error = "insufficient arguments for " + this->prime_flag();
            return 0;
        }
        for (int i = 0; i < this->arg_count; ++i) {
            this->parse_args.push_back(argv[i]);
        }
        this->opt_set = true;
        return this->arg_count;
    }

}

bool flats::initEnv(int argc, char* argv[])
{
    Parser p;
    p.new_option(
            {"-i", "--input-file"},
            [](bool, std::vector<std::string> s)
            {
                if (s.empty()) {
                    return "";
                }
                if (s.size() == 1) {
                    Globals::g.input_file = std::move(s[0]);
                    return "";
                }
                return "duplicate argument for input file";
            })
            ->set_help("Input file, or stdin if not specified")
            ;
    p.new_option(
            {"-e", "--externs", "-E"},
            [](bool, std::vector<std::string> s)
            {
                for (auto const& e: s) {
                    for (std::string s: util::split_str(e, ":")) {
                        Globals::g.external_syms.insert(s);
                    }
                }
                return "";
            })
            ->set_help("Add predefined names (-E is deprecated)")
            ;
    p.new_option(
            {"-p", "--export-point"},
            [](bool, std::vector<std::string> s)
            {
                if (s.empty()) {
                    return "";
                }
                if (s.size() == 1) {
                    Globals::g.export_point = std::move(s[0]);
                    return "";
                }
                return "duplicate argument for export point";
            })
            ->set_help("Override export point for browser env, thus exporting names to window.EXPORT_POINT")
            ;
    p.new_option(
            {"-h", "--help"},
            [&](bool set, std::vector<std::string>)
            {
                if (set) {
                    std::cerr << p.usage() << std::endl;
                    ::exit(0);
                }
                return "";
            })
            ->set_help("Show this help message and exit")
            ->set_arg_count(0)
            ;
    p.parse(argc, argv);
    for (std::string const& i: pre_defined) {
        Globals::g.external_syms.insert(i);
    }
    return p.parsing_done();
}

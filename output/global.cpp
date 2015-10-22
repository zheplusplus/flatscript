#include <misc/version.h>
#include <globals.h>

#include "global.h"

using namespace output;

void output::wrapGlobal(std::ostream& os, util::sref<Statement const> global)
{
    os << "/* Generated by Flatscript:" + misc::version() + " */" << std::endl;
    os << "(function() {" << std::endl;
    if (flats::Globals::g.use_class_ext) {
        os <<
            "function $extend(i, b) {"
                "var base = b.$class;"
                "for (var m in base) {"
                    "if (base.hasOwnProperty(k)) {"
                        "i[k] = base[k];"
                    "}"
                "}"
                "function ctor() {"
                    "this.constructor = i;"
                "}"
                "ctor.prototype = base.prototype;"
                "i.prototype = new ctor();"
                "i.$super = base.prototype;"
                "return i;"
            "}";
    }
    if (flats::Globals::g.use_list_slice) {
        os <<
            "function $listslice(list, begin, end, step) {"
                "function round(x) {"
                    "if (x > list.length) return list.length;"
                    "if (x < 0) return x % list.length + list.length;"
                    "return x;"
                "}"
                "var r = [];"
                "step = step || 1;"
                "if (step > 0) {"
                    "begin = round(begin || 0);"
                    "end = (end === undefined) ? list.length : round(end);"
                    "for (; begin < end; begin += step) {"
                        "r.push(list[begin]);"
                    "}"
                    "return r;"
                "}"
                "begin = (begin === undefined) ? list.length - 1 : round(begin);"
                "end = (end === undefined) ? -1 : round(end);"
                "for (; begin > end; begin += step) {"
                    "r.push(list[begin]);"
                "}"
                "return r;"
            "}";
    }
    global->write(os);
    os << "})();" << std::endl;
}

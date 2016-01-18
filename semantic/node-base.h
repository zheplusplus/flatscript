#ifndef __STEKIN_SEMANTIC_NODE_BASE_H__
#define __STEKIN_SEMANTIC_NODE_BASE_H__

#include <string>
#include <gmpxx.h>

#include <util/pointer.h>
#include <output/node-base.h>
#include <misc/pos-type.h>

namespace semantic {

    struct Scope;
    struct SymbolTable;

    struct Statement {
        misc::position const pos;

        virtual bool isAsync() const = 0;
        virtual void compile(util::sref<Scope> scope) const = 0;

        void compile(Scope& scope) const
        {
            this->compile(util::mkref(scope));
        }

        virtual ~Statement() {}

        explicit Statement(misc::position const& ps)
            : pos(ps)
        {}

        Statement(Statement const&) = delete;
    };

    struct Expression {
        misc::position const pos;

        virtual bool isAsync() const = 0;
        virtual util::sptr<output::Expression const> compile(util::sref<Scope> scope) const = 0;
        virtual util::sptr<output::Expression const> compileAsRoot(util::sref<Scope> scope) const
        {
            return compile(scope);
        }

        virtual bool isLiteral(util::sref<SymbolTable const>) const { return false; }
        virtual std::string literalType(util::sref<SymbolTable const>) const { return ""; }

        virtual bool boolValue(util::sref<SymbolTable const>) const { return false; }
        virtual mpz_class intValue(util::sref<SymbolTable const>) const { return 0; }
        virtual mpf_class floatValue(util::sref<SymbolTable const>) const { return 0; }
        virtual std::string stringValue(util::sref<SymbolTable const>) const { return "";}

        virtual ~Expression() {}

        Expression(misc::position const& ps)
            : pos(ps)
        {}

        Expression(Expression const&) = delete;
    };

}

#endif /* __STEKIN_SEMANTIC_NODE_BASE_H__ */

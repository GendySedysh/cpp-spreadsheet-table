#include "formula.h"
#include "sheet.h"
#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <forward_list>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#ARITHM!";
}

namespace {
class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression) try
        : ast_(ParseFormulaAST(std::move(expression)))
        {} 
        catch (std::exception &exc) {
            throw FormulaException("");
        }

    Value Evaluate(const SheetInterface& sheet) const override {
        try {
            return ast_.Execute(static_cast<const Sheet&>(sheet));
        } catch (FormulaError &exc) {
            auto category = exc.GetCategory();
            return FormulaError(category);
        }
    }

    std::string GetExpression() const override {
        std::stringstream ss;
        ast_.PrintFormula(ss);

        std::string str;
        ss >> str;
        return str;
    }

    std::vector<Position> GetReferencedCells() const override {
        auto tmp = ast_.GetCells();
        tmp.unique();
        return { tmp.begin(), tmp.end() };
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}
#pragma once

#include "common.h"
#include "formula.h"
#include "sheet.h"

class Sheet;

enum CellType {
    EmptyType,
    FunctionType,
    TextType
};

class Cell : public CellInterface {
    class BaseCell {
    public:
        virtual ~BaseCell() = default;

        virtual void Set(std::string text) = 0;
        virtual CellInterface::Value GetValue() = 0;
        virtual std::string GetText() const = 0;
        virtual CellType GetType() const = 0;
    };

    class EmptyCell final : public BaseCell {
    public:
        EmptyCell();
        ~EmptyCell() = default;

        void Set(std::string text) override;
        CellInterface::Value GetValue() override;
        std::string GetText() const override;
        CellType GetType() const override;
    };

    class TextCell final : public BaseCell {
    public:
        TextCell(std::string text);
        ~TextCell() = default;

        void Set(std::string text) override;
        CellInterface::Value GetValue() override;
        std::string GetText() const override;
        CellType GetType() const override;
    private:
        std::string text_;
    };

    class FunctionCell final : public BaseCell {
    public:
        FunctionCell(std::string text, Cell* cell);
        ~FunctionCell() = default;

        void Set(std::string text) override;
        void Set(std::string text, Cell *cell);
        CellInterface::Value GetValue() override;
        std::string GetText() const override;
        CellType GetType() const override;
    private:
        CellInterface::Value data_;
        std::string text_;
    };

public:
    Cell(Sheet& sheet, Position& pos);
    ~Cell() = default;

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;
    CellType GetType() const;
private:
    std::unique_ptr<BaseCell> cell_;
    std::vector<Position> referenced_cells_;
    Sheet& sheet_;
    Position pos_;
};
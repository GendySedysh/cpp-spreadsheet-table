#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

// Empty
Cell::EmptyCell::EmptyCell() {}

void Cell::EmptyCell::Set(std::string text) {}

CellInterface::Value Cell::EmptyCell::GetValue() {
	return "";
}

std::string Cell::EmptyCell::GetText() const {
	return "";
}

CellType Cell::EmptyCell::GetType() const {
	return CellType::EmptyType;
}


// Text
Cell::TextCell::TextCell(std::string text) {
	this->Set(text);
}

void Cell::TextCell::Set(std::string text) {
	text_ = text;
}

CellInterface::Value Cell::TextCell::GetValue() {
	if (text_.front() == ESCAPE_SIGN) {
		return text_.substr(1);
	}
	return text_;
}

std::string Cell::TextCell::GetText() const {
	return text_;
}

CellType Cell::TextCell::GetType() const {
	return CellType::TextType;
}


// Function
Cell::FunctionCell::FunctionCell(std::string text, Cell* cell) {
	this->Set(text, cell);
}

void Cell::FunctionCell::Set(std::string text, Cell* cell) {
	auto FromulaPtr = ParseFormula(text.substr(1));
	cell->referenced_cells_ = std::move(FromulaPtr->GetReferencedCells());

	for (auto& pos : cell->referenced_cells_) {
		if (pos == cell->pos_) {
			throw CircularDependencyException("Selfcycle cell error " + cell->pos_.ToString());
		}

		if (!cell->sheet_.IsCellFulled(pos)) {
			cell->sheet_.InitiateEmpty(pos);
		}
	}

	cell->sheet_.Graph_AddEdges(cell->referenced_cells_, cell->pos_);

	if (cell->sheet_.Graph_SCC()) {
		throw CircularDependencyException("Circular dependecny when try to add: " + cell->pos_.ToString());
	}

	auto evaluated = FromulaPtr->Evaluate(cell->sheet_);
	text_ = FORMULA_SIGN + FromulaPtr->GetExpression();
	if (std::holds_alternative<double>(evaluated)) {
		data_ = std::get<double>(evaluated);
	}
	else {
		data_ = std::get<FormulaError>(evaluated);
	}
}

void Cell::FunctionCell::Set(std::string text) {}

CellInterface::Value Cell::FunctionCell::GetValue() {
	return data_;
}

std::string Cell::FunctionCell::GetText() const {
	return text_;
}

CellType Cell::FunctionCell::GetType() const { return CellType::FunctionType; }

// Cell
Cell::Cell(Sheet& sheet, Position& pos) : sheet_(sheet), pos_(pos) {
	cell_.reset();
	this->referenced_cells_.clear();
}

void Cell::Set(std::string text) {
	this->referenced_cells_.clear();
	if (text.size() != 0) {
		if (text[0] == FORMULA_SIGN && text.size() != 1) {
            cell_ = std::make_unique<FunctionCell>(text, this);
		}
		else {
            cell_ = std::make_unique<TextCell>(text);
		}
	}
	else {
        cell_ = std::make_unique<EmptyCell>();
	}
}

void Cell::Clear() {
    cell_ = std::make_unique<EmptyCell>();
}

Cell::Value Cell::GetValue() const {
	return cell_->GetValue();
}

std::string Cell::GetText() const {
	return cell_->GetText();
}


std::vector<Position> Cell::GetReferencedCells() const {
	return referenced_cells_;
}

CellType Cell::GetType() const {
	return cell_->GetType();
}
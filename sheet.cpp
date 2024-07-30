#include "sheet.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

//Graph

void DepenceGraph::AddEdge(Position& from, Position& to) {
    if (cell_to_id_.count(from) == 0) {
        cell_to_id_[from] = current_id_++;
    }

    if (cell_to_id_.count(to) == 0) {
        cell_to_id_[to] = current_id_++;
    }
    storage_[to].push_back(from);
}

void DepenceGraph::AddEdges(std::vector<Position>& vec, Position& to) {
    storage_[to] = vec;

    if (cell_to_id_.count(to) == 0) {
        cell_to_id_[to] = current_id_++;
    }

    for (auto& pos : vec) {
        if (cell_to_id_.count(pos) == 0) {
            cell_to_id_[pos] = current_id_++;
        }
    }
}

bool DepenceGraph::SCC() {
    int V = cell_to_id_.size();

    std::vector<int> disc(V);
    std::vector<int> low(V);
    std::vector<bool> stackMember(V);

    std::stack<int> st;

    // Initialize disc and low, and stackMember arrays
    for (const auto& [pos, id] : cell_to_id_) {
        disc[id] = -1;
        low[id] = -1;
        stackMember[id] = false;
    }

    // Call the recursive helper function to find strongly
    // connected components in DFS tree with vertex 'i'
    for (const auto& [pos, id] : cell_to_id_) {
        if (disc[id] == -1) {
            if (SCCUtil(pos, disc, low, st, stackMember)) {
                return true;
            }
        }
    }
    return false;
}

bool DepenceGraph::SCCUtil(const Position &u_pos, std::vector<int> &disc,
    std::vector<int> &low, std::stack<int> &st, std::vector<bool> &stackMember) {
    static int time = 0;

    // Initialize discovery time and low value
    int u = cell_to_id_[u_pos];
    disc[u] = low[u] = ++time;
    st.push(u);
    stackMember[u] = true;

    // Go through all vertices adjacent to this
    for (auto i = storage_[u_pos].begin(); i != storage_[u_pos].end(); ++i) {
        Position& v_pos = *i;
        int v = cell_to_id_[v_pos]; // v is current adjacent of 'u'

        if (disc[v] == -1) {
            SCCUtil(v_pos, disc, low, st, stackMember);
            low[u] = std::min(low[u], low[v]);
        }
        else if (stackMember[v] == true)
            low[u] = std::min(low[u], disc[v]);
    }

    int w = 0; // To store stack extracted vertices
    if (low[u] == disc[u]) {
        std::vector<int> strong_con;
        while (st.top() != u) {
            w = (int)st.top();
            strong_con.push_back(w);
            stackMember[w] = false;
            st.pop();
        }
        w = (int)st.top();
        strong_con.push_back(w);
        if (strong_con.size() > 1) {
            return true;
        }
        strong_con.clear();
        stackMember[w] = false;
        st.pop();
    }
    return false;
}

// Sheet

void Sheet::InitiateEmpty(Position pos) {
    if (!IsCellFulled(pos)) {
        cells_[pos] = std::make_unique<Cell>(*this, pos);
    }
}

void Sheet::SetCell(Position pos, std::string text) {
    IsPositionValid(pos);

    if (!IsCellFulled(pos)) {
        cells_[pos] = std::make_unique<Cell>(*this, pos);
    }
    cells_.at(pos)->Set(text);
    positions_.insert(pos);
}

const CellInterface* Sheet::GetCell(Position pos) const {
    return const_cast<Sheet*>(this)->GetCell(pos);
}

CellInterface* Sheet::GetCell(Position pos) {
    IsPositionValid(pos);

    if (IsCellFulled(pos)) {
        return cells_.at(pos).get();
    }
    return nullptr;
}

const Cell* Sheet::GetConcreteCell(Position pos) const {
    return const_cast<Sheet*>(this)->GetConcreteCell(pos);
}

Cell* Sheet::GetConcreteCell(Position pos) {
    IsPositionValid(pos);

    if (IsCellFulled(pos)) {
        return cells_.at(pos).get();
    }
    return nullptr;
}

void Sheet::ClearCell(Position pos) {
    IsPositionValid(pos);

    if (IsCellFulled(pos)) {
        cells_.erase(pos);
        positions_.erase(pos);
    }
}

Size Sheet::GetPrintableSize() const {
    if (positions_.size() == 0) {
        return { 0, 0 };
    }

    auto max_row = *std::max_element(positions_.begin(), positions_.end(),
        [](Position lhs, Position rhs) {
            return lhs.row < rhs.row;
        });

    auto max_col = *std::max_element(positions_.begin(), positions_.end(),
        [](Position lhs, Position rhs) {
            return lhs.col < rhs.col;
        });

    return { max_row.row + 1, max_col.col + 1 };
}

void Sheet::PrintValues(std::ostream& output) const {
    Size print_size = GetPrintableSize();

    for (int row = 0; row < print_size.rows; ++row) {
        bool flag = true;
        for (int col = 0; col < print_size.cols; ++col) {
            Position pos = { row, col };

            if (flag) {
                flag = false;
                PrintCellValue(pos, output);
            }
            else {
                output << "\t";
                PrintCellValue(pos, output);
            }
        }
        output << "\n";
    }
}

void Sheet::PrintTexts(std::ostream& output) const {
    Size print_size = GetPrintableSize();

    for (int row = 0; row < print_size.rows; ++row) {
        bool flag = true;
        for (int col = 0; col < print_size.cols; ++col) {
            Position pos = { row, col };

            if (flag) {
                flag = false;
                PrintCellText(pos, output);
            }
            else {
                output << "\t";
                PrintCellText(pos, output);
            }
        }
        output << "\n";
    }
}

void Sheet::PrintCellValue(Position& pos, std::ostream& output) const {
    if (IsCellFulled(pos)) {
        auto value = cells_.at(pos)->GetValue();
        if (std::holds_alternative<double>(value)) {
            output << std::get<double>(value);
            return;
        }

        if (std::holds_alternative<std::string>(value)) {
            output << std::get<std::string>(value);
            return;
        }

        if (std::holds_alternative<FormulaError>(value)) {
            output << std::get<FormulaError>(value);
            return;
        }
    }
}

void Sheet::PrintCellText(Position& pos, std::ostream& output) const {
    if (IsCellFulled(pos)) {
        output << cells_.at(pos)->GetText();
    }
}

bool Sheet::IsCellFulled(Position pos) const {
    return cells_.count(pos) != 0;
}

void Sheet::IsPositionValid(Position& pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException(pos.ToString());
    }
}

void Sheet::Graph_AddEdge(Position& from, Position& to) {
    graph_.AddEdge(from, to);
}

void Sheet::Graph_AddEdges(std::vector<Position>& vec, Position& to) {
    graph_.AddEdges(vec, to);
}

bool Sheet::Graph_SCC() {
    return graph_.SCC();
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}

#pragma once

#include "cell.h"
#include "common.h"
#include <unordered_map>
#include <set>
#include <functional>
#include <vector>
#include <stack>

class Cell;

class PositionHasher {
public:
    size_t operator()(const Position& p) const {
        return p.row * 7 + p.col * 49;
    }
};

// Работа графа скомунизжена с https://www.geeksforgeeks.org/tarjan-algorithm-find-strongly-connected-components/
class DepenceGraph {
public:
    DepenceGraph() = default;
    ~DepenceGraph() = default;

    void AddEdge(Position& from, Position& to);
    void AddEdges(std::vector<Position>& vec, Position& to);
    bool SCC();
private:
    bool SCCUtil(const Position& u_pos, std::vector<int>& disc,
        std::vector<int>& low, std::stack<int>& st, std::vector<bool>& stackMember);

    std::unordered_map<Position, std::vector<Position>, PositionHasher> storage_;
    std::unordered_map<Position, int, PositionHasher> cell_to_id_;
    int current_id_ = 0;
};

class Sheet : public SheetInterface {
public:
    ~Sheet() = default;

    void InitiateEmpty(Position pos);
    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    const Cell* GetConcreteCell(Position pos) const;
    Cell* GetConcreteCell(Position pos);

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

    void Graph_AddEdge(Position& from, Position& to);
    void Graph_AddEdges(std::vector<Position>& vec, Position& to);
    bool Graph_SCC();

    bool IsCellFulled(Position pos) const;
private:
    void IsPositionValid(Position& pos) const;
    void PrintCellValue(Position& pos, std::ostream& output) const;
    void PrintCellText(Position& pos, std::ostream& output) const;

    std::unordered_map<Position, std::unique_ptr<Cell>, PositionHasher> cells_;
    std::set<Position> positions_;
    DepenceGraph graph_;
};
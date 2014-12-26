#ifndef __PUZZLE_H
#define __PUZZLE_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

namespace Puzzle {

class Descriptor {
public:
    virtual ~Descriptor() {}

    virtual vector<int> Values() const {
        return vector<int>();
    }
    virtual string ToStr(int i) const {
        stringstream ss;
        ss << i;
        return ss.str();
    }
};

class IntRangeDescriptor : public Descriptor {
 public:
    IntRangeDescriptor(int start, int end)
        : start_(start), end_(end) {
    }
    ~IntRangeDescriptor() override {}

    virtual vector<int> Values() const override {
        vector<int> ret;
        for (int i = start_; i <= end_; ++i ) {
            ret.push_back(i);
        }
        return ret;
    }

 private:
    int start_;
    int end_;
};

class StringDescriptor : public Descriptor {
public:
    StringDescriptor() {}
    StringDescriptor(const vector<string>& names) : names_(names) {}
    ~StringDescriptor() override {}

    void SetDescription(int i, string d) { names_[i] = d; }
    string ToStr(int i) const override { return names_[i]; }

    virtual vector<int> Values() const override {
        vector<int> ret;
        for (int i = 0; i < names_.size(); ++i ) {
            ret.push_back(i);
        }
        return ret;
    }

private:
    vector<string> names_;
};

class EntryDescriptor {
public:
    EntryDescriptor() {}
    EntryDescriptor(const Descriptor& id_descriptor,
                    const Descriptor& class_descriptor,
                    const vector<Descriptor>& name_descriptors)
        : id_descriptor_(id_descriptor),
          class_descriptor_(class_descriptor),
          name_descriptors_(name_descriptors) {
    }

    string Id(int id_int) const { return id_descriptor_.ToStr(id_int); }
    string Class(int class_int) const { return class_descriptor_.ToStr(class_int); }
    string Name(int class_int, int name_int) const { return name_descriptors_[class_int].ToStr(name_int); }

    static const EntryDescriptor& Invalid() { return invalid_; }

private:
    Descriptor id_descriptor_;
    Descriptor class_descriptor_;
    vector<Descriptor> name_descriptors_;

    static EntryDescriptor invalid_;
};

class Entry {
public:
    Entry(int id, const vector<int>& classes, const EntryDescriptor& entry_descriptor)
        : id_(id), classes_(classes), entry_descriptor_(entry_descriptor) {
    }
    ~Entry() {}

    int id() const { return id_; }
    bool IsValid() const { return id_ >= 0; }
    int Class(int classname) const {
        return classes_[classname];
    }
    string ToStr() const {
        string ret = entry_descriptor_.Id(id_) + ": ";
        for (int i = 0; i < classes_.size(); ++i) {
            ret += entry_descriptor_.Class(i) + "=" + entry_descriptor_.Name(i, classes_[i]);
        }
        return ret;
    }
    static const Entry& Invalid() { return invalid_; }

private:
    Entry(int id) : id_(id), entry_descriptor_(EntryDescriptor::Invalid()) {}
    int id_;
    vector<int> classes_;
    const EntryDescriptor& entry_descriptor_;

    static Entry invalid_;
};

class Solution {
public:
    Solution(const vector<Entry>& entries) : entries_(entries) {}
    ~Solution() {}

    bool IsValid() const { return entries_.size() > 0; }
    const vector<Entry>& entries() const { return entries_; }
    const Entry& Id(int id) const { return entries_[id]; }
    const Entry& Find(function<bool(const Entry&)> pred) const {
        for (const Entry& e: entries_) {if (pred(e)) {return e;}}
        cerr << "Cannot find and entry for the given predicate" << endl;
        return Entry::Invalid();
    }
    string ToStr() const {
        string ret;
        for  (const Entry& e: entries_ ) { ret += e.ToStr() + "\n"; }
        return ret;
    }
    static const Solution& Invalid() { return invalid_; }

private:
    static Solution invalid_;
    vector<Entry> entries_;
};

class Solver {
    public:
    Solver() {}
    ~Solver() {}

    void SetIdentifiers(const Descriptor& id_descriptor) {
    }
    void AddClass(int class_int, const string& classname, const Descriptor& name_descriptor) {
    }
    void AddPredicate(function<bool(const Entry&)> predicate) {
        AddPredicate([predicate](const Solution& s) {
                return all_of(s.entries().begin(),
                              s.entries().end(),
                              predicate);
            });
    }
    void AddPredicate(function<bool(const Solution&)> predicate) {
        onSolution.push_back(predicate);
    }

    Solution Solve();

    private:
    EntryDescriptor entry_descriptor_;
    vector<Solution> possibleSolutions();

    vector<function<bool(const Solution&)>> onSolution;
};

}  // namespace Puzzle

#endif  // __PUZZLE_H

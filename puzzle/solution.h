#ifndef __PUZZLE_SOLUTION_H
#define __PUZZLE_SOLUTION_H

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
    ~StringDescriptor() override {}

    void SetDescription(int i, const string& d) { names_[i] = d; }
    string ToStr(int i) const override { auto it = names_.find(i); if (it != names_.end()) return it->second; return ""; }

    virtual vector<int> Values() const override {
        vector<int> ret;
        for (unsigned int i = 0; i < names_.size(); ++i ) {
            ret.push_back(i);
        }
        return ret;
    }

private:
    unordered_map<int,string> names_;
};

class EntryDescriptor {
public:
    EntryDescriptor() {}

    void SetIds(const Descriptor* id_descriptor) { id_descriptor_ = id_descriptor; }
    void SetClass(int class_int, const string& class_name, const Descriptor* name_descriptor ) {
        class_descriptor_.SetDescription(class_int, class_name);
        name_descriptors_.resize(class_int + 1);
        name_descriptors_[class_int] = name_descriptor;
    }

    const Descriptor* AllIds() const { return id_descriptor_; }
    const Descriptor* AllClasses() const { return &class_descriptor_; }
    const Descriptor* AllClassValues(int class_int) const { return name_descriptors_[class_int]; }

    string Id(int id_int) const { return id_descriptor_->ToStr(id_int); }
    string Class(int class_int) const { return class_descriptor_.ToStr(class_int); }
    string Name(int class_int, int name_int) const {
        return name_descriptors_[class_int] ? name_descriptors_[class_int]->ToStr(name_int) : ""; }

private:
    const Descriptor* id_descriptor_;
    StringDescriptor class_descriptor_;
    vector<const Descriptor*> name_descriptors_;
};

class Entry {
public:
    Entry(int id, const vector<int>& classes, const EntryDescriptor* entry_descriptor)
        : id_(id), classes_(classes), entry_descriptor_(entry_descriptor) {
    }
    ~Entry() {}

    bool operator==(const Entry& other) const {
        if (this == &other)
            return true;
        if (id_ != other.id_)
            return false;
        return classes_ == other.classes_;
    }

    int id() const { return id_; }
    bool IsValid() const { return id_ >= 0; }
    int Class(int classname) const {
        return classes_[classname];
    }
    void SetClass(int classname, int value) {
        classes_[classname] = value;
    }
    string ToStr() const {
        stringstream ret;
        if (entry_descriptor_ != nullptr) {
            ret << entry_descriptor_->Id(id_);
        } else {
            ret << id_;
        }
        ret << ":";
        for (unsigned int i = 0; i < classes_.size(); ++i) {
            if (entry_descriptor_) {
                ret << " " << entry_descriptor_->Class(i) << "=" << entry_descriptor_->Name(i, classes_[i]);
            } else {
                ret << " " << classes_[i];
            }
        }
        return ret.str();
    }
    static const Entry& Invalid() { return invalid_; }

private:
    Entry(int id) : id_(id), entry_descriptor_(nullptr) {}
    int id_;
    vector<int> classes_;
    const EntryDescriptor* entry_descriptor_;

    static Entry invalid_;
};

class Solution {
public:
    Solution() : Solution(nullptr) {}
    Solution(const vector<Entry>* entries) : entries_(entries) {}
    Solution(const Solution& other)
        : entries_(other.entries_ == nullptr ? nullptr : new vector<Entry>(*other.entries_)) {
        own_entries_ = true;
    }
    ~Solution() {
        if (own_entries_ && entries_ != nullptr) {
            delete entries_;
        }
    }

    bool operator==(const Solution& other) const {
        if (this == &other) {
            return true;
        }
        if (entries_ == nullptr || other.entries_ == nullptr) {
            return entries_ == other.entries_;
        }
        return *entries_ == *other.entries_;
    }

    long long permutation_position() const { return permutation_position_; }
    void set_permutation_position(long long position) { permutation_position_ = position; }

    long long permutation_count() const { return permutation_count_; }
    void set_permutation_count(long long count) { permutation_count_ = count; }

    double completion() const { return static_cast<double>(permutation_position_) / permutation_count_; }

    bool IsValid() const { return entries_ != nullptr; }
    const vector<Entry>& entries() const { return *entries_; }
    const Entry& Id(int id) const { return (*entries_)[id]; }
    const Entry& Find(function<bool(const Entry&)> pred) const {
        for (const Entry& e: *entries_) {if (pred(e)) {return e;}}
        cerr << "Cannot find and entry for the given predicate" << endl;
        return Entry::Invalid();
    }
    string ToStr() const {
        string ret;
        if (entries_ != nullptr) {
            for (const Entry& e: *entries_ ) { ret += e.ToStr() + "\n"; }
        }
        return ret;
    }

private:
    const vector<Entry>* entries_;
    bool own_entries_ = false;
    long long permutation_position_;
    long long permutation_count_;
};

typedef function<bool(const Solution&)> Predicate;

struct SolutionCropper {
    SolutionCropper() {}
    SolutionCropper(string name, Predicate p, const vector<int>& classes)
      : name_(name), p_(p), classes_(classes) {}
    string name_;
    Predicate p_;
    vector<int> classes_;
 };


}  // namespace Puzzle

#endif  // __PUZZLE_SOLUTION_H

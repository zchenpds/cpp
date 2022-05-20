#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

class File;
using FilePtr = std::shared_ptr<File>;

class Find;

class File {
    std::string name_;
    size_t size_;
    const bool is_dir_;
    std::string type_;
    std::map<std::string, FilePtr> children;

    friend class Find;
public:
    File(const std::string& name, size_t size, bool is_dir = false) : size_(size), name_(name), is_dir_(is_dir) {
        size_t pos_dot = name.find_last_of('.');
        type_ = name.substr(pos_dot + 1);
    }

    bool addChild(FilePtr file) {
        if (children.count(file->name_)) return false;
        children[file->name_] = file;
        return true;
    }

    template< class... Args >
    FilePtr addChild(Args&&... args) {
        auto file = std::make_shared<File>(std::forward<Args>(args)...);
        if (addChild(file)) return file;
        else return nullptr;
    }

    void listContent(int indentation = 0) const {
        printf("%*s%s\n", indentation, "", name_.c_str());
        for (const auto& [_, child]: children) {
            child->listContent(indentation + 2);
        }
    }

};


class Find {
    using Filter = std::function<bool(FilePtr)>;
    std::vector<Filter> filters_;
    void find(const File& file, std::vector<FilePtr>& res) const {
        for (const auto& [_, child]: file.children) {
            if (child->is_dir_) {
                find(*child, res);
                continue;
            }
            bool satisfied = true;
            for (const auto& filter: filters_) {
                if (filter(child) == false) {
                    satisfied = false;
                    break;
                }
            }
            if (satisfied) res.push_back(child);
        }
    }
public:
    Find() {}
    Find(int argc, char** argv) {
        for (int i = 0; i < argc; ++i) {
            if (argv[i] == std::string("--size")) {
                if (++i < argc) {
                    int size = atoi(argv[i]);
                    filters_.push_back([size](FilePtr file)->bool{
                        return size > 0 ? file->size_ > size : file->size_ < -size;
                    });
                }
            }
            else if (argv[i] == std::string("--type")) {
                if (++i < argc) {
                    std::string type = argv[i];
                    filters_.push_back([type](FilePtr file)->bool{
                        return file->type_ == type;
                    });
                }
            }
        }
    }
    std::vector<FilePtr> find(const FilePtr& file) {
        std::vector<FilePtr> res;
        find(*file, res);
        return res;
    }
    std::vector<FilePtr> find(const File& file) {
        std::vector<FilePtr> res;
        find(file, res);
        return res;
    }
};



// Compile: g++ -std=c++17 002_find.cpp -o 002_find
// Usage: ./002_find --size -30 -type txt

int main(int argc, char** argv) {
    File root("/", true);
    root.addChild("file1.txt", 34, false);
    root.addChild("file2.txt", 67, false);
    if (auto dir1 = root.addChild("dir1", 0, true)) {
        dir1->addChild("file3.jpg", 12, false);
        dir1->addChild("file4.xls", 78, false);
    }
    root.listContent();
    
    printf("Filtered result:\n");
    auto res = Find(argc, argv).find(root);
    for (auto f: res) {
        f->listContent();
    }
    return 0;
}
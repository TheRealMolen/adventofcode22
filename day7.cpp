#include "pch.h"
#include "harness.h"


struct Dir
{
    struct File
    {
        size_t size;
    };

    string name;
    vector<File> files;
    vector<Dir>  dirs;

    size_t combined_size = 0;

    explicit Dir(const char* name) : name(name) { /**/ }
};

void updateCombinedSize(Dir& dir)
{
    if (dir.combined_size != 0)
        return;

    size_t combined = 0;
    for (auto& file : dir.files)
        combined += file.size;
    for (auto& child : dir.dirs)
    {
        updateCombinedSize(child);
        combined += child.combined_size;
    }

    dir.combined_size = combined;
}

void findAllDirsUnderSize(size_t size, const Dir& dir, vector<const Dir*>& found)
{
    if (dir.combined_size < size)
        found.push_back(&dir);

    for (auto& child : dir.dirs)
        findAllDirsUnderSize(size, child, found);
}

void findAllDirsGESize(size_t size, const Dir& dir, vector<const Dir*>& found)
{
    if (dir.combined_size >= size)
        found.push_back(&dir);

    for (auto& child : dir.dirs)
        findAllDirsGESize(size, child, found);
}

Dir loadFs(const stringlist& input)
{
    Dir root{ "/" };
    vector<Dir*> cwd = { &root };

    for (auto& line : input)
    {
        if (line[0] == '$')
        {
            if (line[2] == 'c')     // cd
            {
                string newDir = line.c_str() + 5;
                if (newDir == "..")
                {
                    cwd.pop_back();
                    continue;
                }
                if (newDir == "/")
                    continue;

                auto found = ranges::find_if(cwd.back()->dirs, [&newDir](auto& dir) { return dir.name == newDir; });
                cwd.push_back(&*found);
            }
        }
        else if (line[0] == 'd')    // dir
        {
            cwd.back()->dirs.emplace_back(line.c_str() + 4);
        }
        else // file
        {
            cwd.back()->files.emplace_back(stoull(line));
        }
    }

    updateCombinedSize(root);

    return root;
}


size_t day7(const stringlist& input)
{
    Dir root = loadFs(input);

    vector<const Dir*> smallDirs;
    findAllDirsUnderSize(100001, root, smallDirs);

    return accumulate(begin(smallDirs), end(smallDirs), 0ull, [](size_t acc, const Dir* d) { return acc + d->combined_size; });
}

size_t day7_2(const stringlist& input)
{
    Dir root = loadFs(input);
    constexpr size_t DriveSize =     70000000;
    constexpr size_t SpaceRequired = 30000000;

    size_t spaceFree = DriveSize - root.combined_size;
    size_t deletionRequired = SpaceRequired - spaceFree;

    vector<const Dir*> bigDirs;
    findAllDirsGESize(deletionRequired, root, bigDirs);

    const Dir* smallest = ranges::min(bigDirs, {}, [](const Dir* d){ return d->combined_size; });

    return smallest->combined_size;
}


void run_day7()
{
    string sample =
        R"($ cd /
$ ls
dir a
14848514 b.txt
8504156 c.dat
dir d
$ cd a
$ ls
dir e
29116 f
2557 g
62596 h.lst
$ cd e
$ ls
584 i
$ cd ..
$ cd ..
$ cd d
$ ls
4060174 j
8033020 d.log
5626152 d.ext
7214296 k)";

    test(95437, day7(READ(sample)));
    gogogo(day7(LOAD(7)));

    test(24933642, day7_2(READ(sample)));
    gogogo(day7_2(LOAD(7)));
}
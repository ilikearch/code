#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include "Log.hpp"
#include "Common.hpp"

const std::string gpath = "./";
const std::string gdictname = "dict.txt";
const std::string gsep = ": ";

using namespace LogModule;

class Dictionary
{
private:
    bool LoadDictionary()
    {
        std::string file = _path + _filename;
        std::ifstream in(file.c_str());
        if (!in.is_open())
        {
            LOG(LogLevel::ERROR) << "open file " << file << " error";
            return false;
        }

        std::string line;
        while (std::getline(in, line))
        {
            // happy: 快乐的
            std::string key;
            std::string value;
            if (SplitString(line, &key, &value, gsep))
            { // line -> key, value
                _dictionary.insert(std::make_pair(key, value));
            }
        }

        in.close();
        return true;
    }

public:
    Dictionary(const std::string &path = gpath, const std::string &filename = gdictname)
        : _path(path),
          _filename(filename)
    {
        LoadDictionary();
        Print();
    }
    std::string Translate(const std::string &word)
    {
        auto iter = _dictionary.find(word);
        if(iter == _dictionary.end()) return "None";
        return iter->second;
    }
    void Print()
    {
        for(auto &item : _dictionary)
        {
            std::cout << item.first << ":" << item.second<< std::endl;
        }
    }
    ~Dictionary()
    {
    }

private:
    std::unordered_map<std::string, std::string> _dictionary;
    std::string _path;
    std::string _filename;
};
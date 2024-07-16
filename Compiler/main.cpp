//
//  main.cpp
//  Compiler
//
//  Created by Nathan Thurber on 24/6/24.
//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include "Tokenization.hpp"
#include "Parser.hpp"
#include "Generation.hpp"
#include "Arena.hpp"

int main(int argc, const char * argv[]) {
    std::string fileName;
    std::cin >> fileName;
    
    if (fileName.substr(fileName.size() - 7, fileName.size()) != ".newton")
        std::cerr << "Invalid file format" << std::endl;
    
    std::string contents;
    {
        std::fstream file(fileName, std::ios::in);
        std::stringstream contents_stream;
        contents_stream << file.rdbuf();
        file.close();
        contents = contents_stream.str();
    }
    
    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> Tokens = tokenizer.tokenize();
    
    Parser parser(std::move(Tokens));
    std::optional<NodeProg> prog = parser.parse_prog();
    
    if (!prog.has_value())
        std::cerr << "Invalid Program" << std::endl;
    
    {
        Generator generator(prog.value());
        std::fstream file("/Users/nathan/Documents/Coding/Compiler/out.asm", std::ios::out);
        file << generator.gen_prog();
    }
    
    return 0;
}

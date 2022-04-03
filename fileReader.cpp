//
// Created by hunter on 3/30/22.
//
#include <fstream>
#include <string>
#include <vector>
#include "fileReader.h"
#include "pokemon.h"

int openFile(std::string fileName, std::ifstream &f) {

    f.open("/share/cs327/pokedex/pokedex/data/csv/" + fileName);

    // Try to open the home path
    if (!f.is_open()) {
        std::string home = getenv("HOME");

        std::string path = home + "/.poke327/pokedex/pokedex/data/csv/" + fileName;

        f.open(path);
    }

    // Try to open the home path
    if (!f.is_open()) {

        f.open("/home/hunter/code/C/pokeProject/pokedex/pokedex/data/csv/" + fileName);
    }

    // The big wuh oh
    if (!f.is_open()) {
        return -1;
    }

    return 0;
}

int readAllPokemon(std::vector<pokemon> &list) {
    std::ifstream myFile;

    openFile("pokemon.csv", myFile);

    std::string id_str, identifier, species_id_str, height_str, weight_str, base_experience_str, order_str, is_default_str;
    int id, species_id, height, weight, base_experience, order, is_default;

    getline(myFile, id_str); // throw away the first line

    while (myFile.peek() != std::ifstream::traits_type::eof()) {
        getline(myFile, id_str, ',');
        getline(myFile, identifier, ',');
        getline(myFile, species_id_str, ',');
        getline(myFile, height_str, ',');
        getline(myFile, weight_str, ',');
        getline(myFile, base_experience_str, ',');
        getline(myFile, order_str, ',');
        getline(myFile, is_default_str, '\n');

        id = std::stoi(id_str);
        species_id = std::stoi(species_id_str);
        height = std::stoi(height_str);
        weight = std::stoi(weight_str);
        base_experience = std::stoi(base_experience_str);
        order = std::stoi(order_str);
        is_default = std::stoi(is_default_str);

        pokemon p(id, identifier, species_id, height, weight, base_experience, order, is_default);

        list.push_back(p);
    }

    myFile.close();

    return 0;
}

int readAllPokemonSpecies(std::vector<pokemon_species> &list) {
    std::ifstream myFile;

    openFile("pokemon_species.csv", myFile);

    std::string identifier;

    int id, generation_id, evolves_from_species_id, evolution_chain_id, color_id, shape_id, habitat_id, gender_rate, capture_rate, base_happiness,
    is_baby, hatch_counter, has_gender_differences, growth_rate_id, forms_switchable, is_legendary, is_mythical, order, conquest_order;

    getline(myFile, identifier); // throw away the first line

    while (myFile.peek() != std::ifstream::traits_type::eof()) {
        std::vector<std::string> params;

        std::string str;

        for (int i = 0; i < 19; i++) {
            getline(myFile, str, ',');

            params.push_back(str);
        }

        getline(myFile, str, '\n'); // grab the last one
        params.push_back(str);

        for (int i = 0; i < (int) params.size(); i++) {
            if (params[i] == "") {
                params[i] = "-1";
            }
        }

        identifier = params[1];

        id = std::stoi(params[0]);
        generation_id = std::stoi(params[2]);
        evolves_from_species_id = std::stoi(params[3]);
        evolution_chain_id = std::stoi(params[4]);
        color_id = std::stoi(params[5]);
        shape_id = std::stoi(params[6]);
        habitat_id = std::stoi(params[7]);
        gender_rate = std::stoi(params[8]);
        capture_rate = std::stoi(params[9]);
        base_happiness = std::stoi(params[10]);
        is_baby = std::stoi(params[11]);
        hatch_counter = std::stoi(params[12]);
        has_gender_differences = std::stoi(params[13]);
        growth_rate_id = std::stoi(params[14]);
        forms_switchable = std::stoi(params[15]);
        is_legendary = std::stoi(params[16]);
        is_mythical = std::stoi(params[17]);
        order = std::stol(params[18]);
        conquest_order = std::stol(params[19]);

        pokemon_species p(id, identifier, generation_id, evolves_from_species_id, evolution_chain_id, color_id, shape_id,
                          habitat_id, gender_rate, capture_rate, base_happiness, is_baby, hatch_counter, has_gender_differences,
                          growth_rate_id,forms_switchable, is_legendary, is_mythical, order, conquest_order);

        list.push_back(p);
    }

    myFile.close();

    return 0;
}

int readAllExperience(std::vector<experience> &list) {
    std::ifstream myFile;

    openFile("experience.csv", myFile);

    std::string growth_rate_id_str, level_str, experience_str;

    int growth_rate_id, level, experienceAmt;

    getline(myFile, growth_rate_id_str);

    while (myFile.peek() != std::ifstream::traits_type::eof()) {
        getline(myFile, growth_rate_id_str, ',');
        getline(myFile, level_str, ',');
        getline(myFile, experience_str, '\n');

        growth_rate_id = std::stoi(growth_rate_id_str);
        level = std::stoi(level_str);
        experienceAmt = std::stoi(experience_str);

        experience ex(growth_rate_id, level, experienceAmt);

        list.push_back(ex);
    }

    return 0;
}

int readAllTypes(std::vector<type_names> &list) {
    std::ifstream myFile;

    openFile("type_names.csv", myFile);

    std::string type_id_str, local_language_id_str, name;

    int type_id, local_lang_id;

    getline(myFile, type_id_str);

    while (myFile.peek() != std::ifstream::traits_type::eof()) {
        getline(myFile, type_id_str, ',');
        type_id = std::stoi(type_id_str);

        getline(myFile, local_language_id_str, ',');
        local_lang_id = std::stoi(local_language_id_str);

        getline(myFile, name, '\n');

        if (local_lang_id == 9) {
            type_names t(type_id, local_lang_id, name);

            list.push_back(t);
        }
    }

    return 0;
}

int readAllPokemonMoves(std::vector<pokemon_moves> &list) {
    std::ifstream myFile;

    openFile("pokemon_moves.csv", myFile);

    std::string str;

    getline(myFile, str); // throw away the first line

    while (myFile.peek() != std::ifstream::traits_type::eof()) {
        std::vector<int> params;

        for (int i = 0; i < 5; i++) {
            getline(myFile, str, ',');

            if (str != "") {
                params.push_back(std::stoi(str));
            } else {
                params.push_back(-1);
            }
        }

        if (myFile.peek() != '\n') {
            getline(myFile, str, '\n');

            params.push_back(std::stoi(str));
        } else {
            getline(myFile, str, '\n'); // get rid of the \n
            params.push_back(-1);
        }

        pokemon_moves pm(params[0], params[1], params[2], params[3], params[4], params[5]);

        list.push_back(pm);
    }

    return 0;
}

int readAllMoves(std::vector<moves> &list) {
    std::ifstream myFile;

    openFile("moves.csv", myFile);

    std::string str, identifier;

    getline(myFile, str); // throw away the first line

    while (myFile.peek() != std::ifstream::traits_type::eof()) {
        std::vector<int> params;

        for (int i = 0; i < 14; i++) {
            if (i != 1) {
                getline(myFile, str, ',');

                if (str != "") {
                    params.push_back(std::stoi(str));
                } else {
                    params.push_back(-1);
                }
            } else {
                getline(myFile, identifier, ',');
            }
        }

        if (myFile.peek() != '\n' && myFile.peek() != std::ifstream::traits_type::eof()) {
            getline(myFile, str, '\n');

            params.push_back(std::stoi(str));
        } else {
            getline(myFile, str, '\n');
            params.push_back(-1);
        }

        moves m(params[0], identifier, params[1], params[2], params[3], params[4], params[5], params[6], params[7],
                params[8], params[9], params[10], params[11], params[12], params[13]);

        list.push_back(m);
    }

    return 0;
}
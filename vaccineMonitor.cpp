#include <iostream>
#include <string>
#include <fstream>

#include "vaccineMonitor.h"
#include "util.h"
#include "DataStructures/date/date.h"
#include "DataStructures/binaryAvlTree/tree.h"
#include "DataStructures/linkedList/linkedListString.h"
#include "DataStructures/bloomFilter/bloomFilter.h"
#include "DataStructures/skipList/skipList.h"
#include "citizenRecords/citizen.h"
#include "DataStructures/population/population.h"

using namespace std;

vaccineMonitor::vaccineMonitor(int bloomSize)
{
    this->tree = NULL;
    this->countryList = new linkedListStringNode();
    this->virusList = new linkedListStringNode();
    this->bloomList = new bloomFilterList(bloomSize);
    this->skiplist = new skipList_Lists();
}

vaccineMonitor::~vaccineMonitor()
{
    delete tree;
    delete countryList;
    delete virusList;
    delete bloomList;
    delete skiplist;
}

void vaccineMonitor::addFromFile(string filepath)
{
    ifstream file(filepath);
    string line;
    while (getline(file, line))
    {
        int length;
        string *words = splitString(line, &length);
        addRecord(length, words, line);
        delete[] words;
    }
    file.close();
}

void vaccineMonitor::addRecord(int length, string *words, string line)
{

    citizenRecord *citizen;

    if (this->checkSyntaxRecord("ERROR IN RECORD ", length, words, line)) // the record had syntax errors
    {
        return;
    }

    addNewVirus(words[5]);
    linkedListStringNode *virus = virusList->search(words[5]);
    addNewCountry(words[3]);
    linkedListStringNode *country = countryList->search(words[3]);

    char status = 'n';
    string date = "";
    if (words[6].compare("YES") == 0)
    {
        status = 'y';
        date = words[7];
    }

    citizen = new citizenRecord(stoi(words[0]), words[1], words[2], country, stoi(words[4]), virus, status, date);

    string result = "";
    citizenRecord *alreadyInTree = NULL;

    tree = tree->insert(tree, citizen, &alreadyInTree, &result, false); // insert in tree

    if (result.compare("NEW CITIZEN") == 0)
    {
        if (status == 'y')
        {
            bloomList->getBloom(virus)->add(citizen->getID());
            skiplist->getVaccinated(virus)->add(citizen->getID(), citizen);
        }
        else if (status == 'n')
        {
            skiplist->getNotVaccinated(virus)->add(citizen->getID(), citizen);
        }
    }
    else if (result.compare("VIRUS ADDED TO CITIZEN") == 0)
    {
        if (status == 'y')
        {
            bloomList->getBloom(virus)->add(alreadyInTree->getID());
            skiplist->getVaccinated(virus)->add(alreadyInTree->getID(), alreadyInTree);
        }
        else if (status == 'n')
        {
            skiplist->getNotVaccinated(virus)->add(alreadyInTree->getID(), alreadyInTree);
        }
    }
    else if (result.compare("WRONG CREDENTIALS") == 0)
    {
        if (length == 8)
        {
            cout << "ERROR IN RECORD " << words[0] << " " << words[1] << " " << words[2] << " " << words[3] << " " << words[4] << " " << words[5] << " " << words[6] << " " << words[7] << endl;
        }
        else
        {
            cout << "ERROR IN RECORD " << words[0] << " " << words[1] << " " << words[2] << " " << words[3] << " " << words[4] << " " << words[5] << " " << words[6] << endl;
        }
        cout << "ERROR= " << result << endl;
    }
    else if (result.compare("VIRUS DUPLICATE") == 0)
    {
        if (length == 8)
        {
            cout << "ERROR IN RECORD " << words[0] << " " << words[1] << " " << words[2] << " " << words[3] << " " << words[4] << " " << words[5] << " " << words[6] << " " << words[7] << endl;
        }
        else
        {
            cout << "ERROR IN RECORD " << words[0] << " " << words[1] << " " << words[2] << " " << words[3] << " " << words[4] << " " << words[5] << " " << words[6] << endl;
        }
        cout << "ERROR= " << result << endl;
    }
}

void vaccineMonitor::startMenu()
{
    do
    {
        cout << endl;
        cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
        cout << "Available commands" << endl;
        cout << "   /vaccineStatusBloom citizenID virusName" << endl;
        cout << "   /vaccineStatus citizenID [virusName]" << endl;
        cout << "   /populationStatus [country] virusName date1 date2" << endl;
        cout << "   /popStatusByAge [country] virusName date1 date2" << endl;
        cout << "   /insertCitizenRecord citizenID firstName lastName country age virusName YES/NO [date] " << endl;
        cout << "   /vaccinateNow citizenID firstName lastName country age virusName" << endl;
        cout << "   /list-nonVaccinated-Persons virusName " << endl;
        cout << "   /exit" << endl;
        cout << endl;
        string input = getInput("Enter command: ");
        int length;
        string *command = splitString(input, &length);
        if (length > 0)
        {
            if (command[0].compare("/vaccineStatusBloom") == 0)
            {
                vaccineStatusBloom(command, length);
            }
            else if (command[0].compare("/vaccineStatus") == 0)
            {
                vaccineStatus(command, length);
            }
            else if (command[0].compare("/populationStatus") == 0)
            {
                populationStatus(command, length);
            }
            else if (command[0].compare("/popStatusByAge") == 0)
            {
                popStatusByAge(command, length);
            }
            else if (command[0].compare("/insertCitizenRecord") == 0)
            {
                insertCitizenRecord(input);
            }
            else if (command[0].compare("/vaccinateNow") == 0)
            {
                vaccinateNow(command, length);
            }
            else if (command[0].compare("/list-nonVaccinated-Persons") == 0)
            {
                listNonVaccinatedPersons(command, length);
            }
            else if (command[0].compare("/exit") == 0)
            {
                cout << "exit" << endl;
                delete[] command;
                break;
            }
            else
            {
                cout << "Invalid command!" << endl;
            }
        }
        else
        {
        }
        delete[] command;
    } while (1);
}

int vaccineMonitor::checkSyntaxRecord(string errorMessage, int length, string *words, string input)
{
    if (length < 7 || length > 8) // record must be only 7 or 8 words
    {
        errorMessage.pop_back();
        cout << errorMessage;

        for (int i = 0; i < length; i++)
        {
            cout << " " << words[i];
        }
        cout << endl;
        cout << "ERROR= ARGUMENT LENGTH ERROR" << endl;
        return 1;
    }
    if (stoi(words[0]) > 9999 && stoi(words[0]) <= 0) // id is 4 digits
    {
        cout << errorMessage << input << endl;
        cout << "ERROR= ID ERROR" << endl;
        return 1;
    }
    if (stoi(words[4]) > 120 && stoi(words[4]) <= 0) // age is from 1 to 120
    {
        cout << errorMessage << input << endl;
        cout << "ERROR= AGE ERROR" << endl;
        return 1;
    }
    if (words[6].compare("NO") != 0 && words[6].compare("YES") != 0) // the 6 word must only be NO or YES
    {
        cout << errorMessage << input << endl;
        cout << "ERROR= YES/NO ERROR" << endl;
        return 1;
    }
    if (length == 8 && (words[6].compare("NO") == 0)) // when we have 8 words the word must be YES
    {
        cout << errorMessage << words[0] << " " << words[1] << " " << words[2] << " " << words[3] << " " << words[4] << " " << words[5] << " " << words[6] << " " << words[7] << endl;
        cout << "ERROR= NO WITH DATE ERROR" << endl;
        return 1;
    }
    if (length == 7 && (words[6].compare("YES") == 0)) // when we have 7 words the word must be NO
    {
        cout << errorMessage << words[0] << " " << words[1] << " " << words[2] << " " << words[3] << " " << words[4] << " " << words[5] << " " << words[6] << endl;
        cout << "ERROR= YES WITHOUT DATE ERROR" << endl;
        return 1;
    }
    if (length == 8)
    {
        date checker(words[7]);
        if (!checker.isValid())
        {
            cout << errorMessage << words[0] << " " << words[1] << " " << words[2] << " " << words[3] << " " << words[4] << " " << words[5] << " " << words[6] << " " << words[7] << endl;
            cout << "ERROR= DATE FORMAT" << endl;
            return 1;
        }
    }
    return 0;
}

void vaccineMonitor::addNewVirus(string virusName)
{

    if (virusList->search(virusName) == NULL) // if we dont have that virus add it to the list of viruses
    {                                         // and make the bloom filter and the skiplist for that virus
        virusList = virusList->add(virusName);
        bloomList = bloomList->add(virusList);
        skiplist = skiplist->add(virusList);
    }
}

void vaccineMonitor::addNewCountry(string countryName)
{
    if (countryList->search(countryName) == NULL) // if we dont have that country add it to the list of Countries
    {
        countryList = countryList->add(countryName);
    }
}

// COMMANDS
void vaccineMonitor::vaccineStatusBloom(string *arguments, int length)
{
    cout << endl;
    cout << "- Selected: /vaccineStatusBloom" << endl;
    if (length == 3)
    {
        int id;
        string virus = "";
        try
        {
            id = stoi(arguments[1]);
            virus = arguments[2];
        }
        catch (std::exception &e)
        {
            cout << "ERROR citizenID must be given as first argument && it must be an integer" << endl;
            cout << "ERROR virusName must be given as second argument" << endl;
            return;
        }
        cout << "- citizenID: " << id << endl;
        cout << "- virusName: " << virus << endl;
        cout << endl;
        int res;
        linkedListStringNode *v = virusList->search(virus);
        if (v != NULL)
        {
            res = bloomList->getBloom(v)->check(id);
        }
        else
        {
            cout << "NOT VACCINATED" << endl;
            return;
        }

        if (res)
        {
            cout << "MAYBE" << endl;
            return;
        }
        else
        {
            cout << "NOT VACCINATED" << endl;
            return;
        }
    }
    else
    {
        cout << "ERROR 2 Arguments must be given" << endl;
        cout << "ERROR Total arguments given was: " << length - 1 << endl;
        return;
    }
}

void vaccineMonitor::vaccineStatus(string *arguments, int length)
{
    cout << endl;
    cout << "- Selected: /vaccineStatus" << endl;
    if (length == 2 || length == 3)
    {
        int id;
        string virus = "";
        try
        {
            id = stoi(arguments[1]);
            if (length == 3)
            {
                virus = arguments[2];
            }
        }
        catch (std::exception &e)
        {
            cout << "ERROR citizenID must be given as first argument && it must be an integer" << endl;
            if (length == 3)
            {
                cout << "ERROR virusName must be given as second argument" << endl;
            }
            return;
        }
        cout << "- citizenID: " << id << endl;
        if (length == 3)
        {
            cout << "- virusName: " << virus << endl;
        }
        cout << endl;

        linkedListStringNode *v = virusList;
        do
        {
            if (v == NULL)
            {
                break;
            }
            if (length == 3)
            {
                v = virusList->search(virus);
            }
            skipList *l;
            if (v != NULL)
            {
                l = skiplist->getVaccinated(v);
                skipListNode *n = l->search(id, 't');
                if (n == NULL)
                {
                    if (length == 2)
                    {
                        cout << v->getString() << " NO" << endl;
                    }
                    else
                    {
                        cout << "NOT VACCINATED" << endl;
                    }
                }
                else
                {
                    if (length == 2)
                    {
                        cout << v->getString() << " YES ";
                        n->getCitizen()->getStatus()->getVirusDate(v).print();
                        cout << endl;
                    }
                    else
                    {
                        cout << "VACCINATED ON ";
                        n->getCitizen()->getStatus()->getVirusDate(v).print();
                        cout << endl;
                    }
                }
            }
            else
            {
                cout << "NOT VACCINATED" << endl;
            }
            if (length == 3)
            {
                break;
            }
            v = v->getNext();
        } while (1);
    }
    else
    {
        cout << "ERROR 1 or 2 Arguments must be given" << endl;
        cout << "ERROR Total arguments given was: " << length - 1 << endl;
        return;
    }
}

void vaccineMonitor::treeInOrderPopulationCountry(treeNode *node, population **stat, linkedListStringNode *country, linkedListStringNode *virus, date date1, date date2)
{
    if (node == NULL)
        return;
    treeInOrderPopulationCountry(node->getLeft(), stat, country, virus, date1, date2);

    citizenRecord *citizen = node->getCitizen();
    if (citizen->getCountry()->getString().compare(country->getString()) == 0)
    {
        char citizenStatus = citizen->getStatus()->getVirusStatus(virus);
        date citizenDate = citizen->getStatus()->getVirusDate(virus);
        if (citizenStatus == 'y')
        {
            if (citizenDate.compare(date1) == 1 && citizenDate.compare(date2) == -1)
            {
                (*stat)->inYes(0);
            }
            else
            {
                (*stat)->outYes(0);
            }
        }
        else if (citizenStatus == 'n')
        {
            (*stat)->NO(0);
        }
        else if (citizenStatus == '\0')
        {
            (*stat)->noInfo(0);
        }
    }

    treeInOrderPopulationCountry(node->getRight(), stat, country, virus, date1, date2);
}

void vaccineMonitor::treeInOrderPopulationGlobal(treeNode *node, population **stat, linkedListStringNode *virus, date date1, date date2)
{
    if (node == NULL)
        return;
    treeInOrderPopulationGlobal(node->getLeft(), stat, virus, date1, date2);

    citizenRecord *citizen = node->getCitizen();

    population *popCountry = (*stat)->find(citizen->getCountry());

    char citizenStatus = citizen->getStatus()->getVirusStatus(virus);
    date citizenDate = citizen->getStatus()->getVirusDate(virus);

    if (citizenStatus == 'y')
    {
        if (citizenDate.compare(date1) == 1 && citizenDate.compare(date2) == -1)
        {
            popCountry->inYes(0);
        }
        else
        {
            popCountry->outYes(0);
        }
    }
    else if (citizenStatus == 'n')
    {
        popCountry->NO(0);
    }
    else if (citizenStatus == '\0')
    {
        popCountry->noInfo(0);
    }

    treeInOrderPopulationGlobal(node->getRight(), stat, virus, date1, date2);
}

void vaccineMonitor::populationStatus(string *arguments, int length)
{
    cout << "Selected: /populationStatus" << endl;
    if (length == 4 || length == 5)
    {
        date date1("");
        date date2("");
        if (length == 5)
        {
            date1.setAll(arguments[3]);
            date2.setAll(arguments[4]);
        }
        else
        {
            date1.setAll(arguments[2]);
            date2.setAll(arguments[3]);
        }
        if (!date1.isValid() || !date2.isValid())
        {
            cout << "ERROR Dates are not in valid format" << endl;
            return;
        }
        else if (date1.compare(date2) == 1)
        {
            cout << "ERROR date1 > date2" << endl;
            return;
        }
        linkedListStringNode *country;
        linkedListStringNode *virus;
        if (length == 5)
        {
            cout << "- country: " << arguments[1] << endl;
            cout << "- virusName: " << arguments[2] << endl;
            cout << "- date1: " << arguments[3] << endl;
            cout << "- date2: " << arguments[4] << endl;
            cout << endl;

            if (this->countryList->search(arguments[1]) == NULL)
            { // not in countyList
                // cout << "1" << endl;
                cout << arguments[1] << " 0 0%" << endl;
                return;
            }
            else
            {
                country = countryList->search(arguments[1]);
                if (this->virusList->search(arguments[2]) == NULL)
                {
                    // cout << "2" << endl;
                    cout << arguments[1] << " 0 0%" << endl;
                    return;
                }
                else
                {
                    // cout << "3" << endl;
                    virus = virusList->search(arguments[2]);

                    population *stat = new population(country, false);
                    this->treeInOrderPopulationCountry(tree, &stat, country, virus, date1, date2);
                    stat->print();
                    delete stat;
                    return;
                }
            }
        }
        else
        {
            cout << "- virusName: " << arguments[1] << endl;
            cout << "- date1: " << arguments[2] << endl;
            cout << "- date2: " << arguments[3] << endl;
            cout << endl;

            if (this->virusList->search(arguments[1]) == NULL)
            { // not in virusList
                linkedListStringNode *temp = countryList;
                while (temp != NULL)
                {
                    cout << temp->getString() << " 0 0%" << endl;
                    temp = temp->getNext();
                }
                return;
            }
            else
            {
                virus = virusList->search(arguments[1]);

                linkedListStringNode *temp = countryList;
                population *stat = new population(temp, false);
                temp = temp->getNext();
                while (temp != NULL)
                {
                    population *newNode = new population(temp, false);
                    newNode->setNext(stat);
                    stat = newNode;
                    temp = temp->getNext();
                }

                this->treeInOrderPopulationGlobal(tree, &stat, virus, date1, date2);
                stat->print();
                delete stat;
                return;
                return;
            }
        }
    }
    else
    {
        cout << "ERROR 3 or 4 Arguments must be given" << endl;
        cout << "ERROR Total arguments given was: " << length - 1 << endl;
        return;
    }
}

void vaccineMonitor::treeInOrderPopulationByAgeCountry(treeNode *node, population **stat, linkedListStringNode *country, linkedListStringNode *virus, date date1, date date2)
{
    if (node == NULL)
        return;
    treeInOrderPopulationByAgeCountry(node->getLeft(), stat, country, virus, date1, date2);

    citizenRecord *citizen = node->getCitizen();
    if (citizen->getCountry()->getString().compare(country->getString()) == 0)
    {
        int age = citizen->getAge();
        int position;
        if (age <= 19) // 0-20
        {
            position = 0;
        }
        else if (age <= 39) // 20-40
        {
            position = 1;
        }
        else if (age <= 59) // 40-60
        {
            position = 2;
        }
        else // 60+
        {
            position = 3;
        }
        char citizenStatus = citizen->getStatus()->getVirusStatus(virus);
        date citizenDate = citizen->getStatus()->getVirusDate(virus);
        if (citizenStatus == 'y')
        {
            if (citizenDate.compare(date1) == 1 && citizenDate.compare(date2) == -1)
            {
                (*stat)->inYes(position);
            }
            else
            {
                (*stat)->outYes(position);
            }
        }
        else if (citizenStatus == 'n')
        {
            (*stat)->NO(position);
        }
        else if (citizenStatus == '\0')
        {
            (*stat)->noInfo(position);
        }
    }

    treeInOrderPopulationByAgeCountry(node->getRight(), stat, country, virus, date1, date2);
}

void vaccineMonitor::treeInOrderPopulationByAgeGlobal(treeNode *node, population **stat, linkedListStringNode *virus, date date1, date date2)
{
    if (node == NULL)
        return;
    treeInOrderPopulationByAgeGlobal(node->getLeft(), stat, virus, date1, date2);

    citizenRecord *citizen = node->getCitizen();
    int age = citizen->getAge();
    int position;
    if (age <= 19) // 0-20
    {
        position = 0;
    }
    else if (age <= 39) // 20-40
    {
        position = 1;
    }
    else if (age <= 59) // 40-60
    {
        position = 2;
    }
    else // 60+
    {
        position = 3;
    }
    population *popCountry = (*stat)->find(citizen->getCountry());

    char citizenStatus = citizen->getStatus()->getVirusStatus(virus);
    date citizenDate = citizen->getStatus()->getVirusDate(virus);

    if (citizenStatus == 'y')
    {
        if (citizenDate.compare(date1) == 1 && citizenDate.compare(date2) == -1)
        {
            popCountry->inYes(position);
        }
        else
        {
            popCountry->outYes(position);
        }
    }
    else if (citizenStatus == 'n')
    {
        popCountry->NO(position);
    }
    else if (citizenStatus == '\0')
    {
        popCountry->noInfo(position);
    }

    treeInOrderPopulationByAgeGlobal(node->getRight(), stat, virus, date1, date2);
}

void vaccineMonitor::popStatusByAge(string *arguments, int length)
{
    cout << "Selected: /popStatusByAge" << endl;
    if (length == 4 || length == 5)
    {
        date date1("");
        date date2("");
        if (length == 5)
        {
            date1.setAll(arguments[3]);
            date2.setAll(arguments[4]);
        }
        else
        {
            date1.setAll(arguments[2]);
            date2.setAll(arguments[3]);
        }
        if (!date1.isValid() || !date2.isValid())
        {
            cout << "ERROR Dates are not in valid format" << endl;
            return;
        }
        else if (date1.compare(date2) == 1)
        {
            cout << "ERROR date1 > date2" << endl;
            return;
        }
        linkedListStringNode *country;
        linkedListStringNode *virus;
        if (length == 5)
        {
            cout << "- country: " << arguments[1] << endl;
            cout << "- virusName: " << arguments[2] << endl;
            cout << "- date1: " << arguments[3] << endl;
            cout << "- date2: " << arguments[4] << endl;
            cout << endl;

            if (this->countryList->search(arguments[1]) == NULL)
            { // not in countyList
                // cout << "1" << endl;
                cout << arguments[1] << " 0 0%" << endl;
                return;
            }
            else
            {
                country = countryList->search(arguments[1]);
                if (this->virusList->search(arguments[2]) == NULL)
                {
                    // cout << "2" << endl;
                    cout << arguments[1] << " 0 0%" << endl;
                    return;
                }
                else
                {
                    // cout << "3" << endl;
                    virus = virusList->search(arguments[2]);

                    population *stat = new population(country, true);
                    this->treeInOrderPopulationByAgeCountry(tree, &stat, country, virus, date1, date2);
                    stat->print();
                    delete stat;
                    return;
                }
            }
        }
        else
        {
            cout << "- virusName: " << arguments[1] << endl;
            cout << "- date1: " << arguments[2] << endl;
            cout << "- date2: " << arguments[3] << endl;
            cout << endl;

            if (this->virusList->search(arguments[1]) == NULL)
            { // not in virusList
                linkedListStringNode *temp = countryList;
                while (temp != NULL)
                {
                    cout << temp->getString() << " 0 0%" << endl;
                    temp = temp->getNext();
                }
                return;
            }
            else
            {
                virus = virusList->search(arguments[1]);

                linkedListStringNode *temp = countryList;
                population *stat = new population(temp, true);
                temp = temp->getNext();
                while (temp != NULL)
                {
                    population *newNode = new population(temp, true);
                    newNode->setNext(stat);
                    stat = newNode;
                    temp = temp->getNext();
                }

                this->treeInOrderPopulationByAgeGlobal(tree, &stat, virus, date1, date2);
                stat->print();
                delete stat;
                return;
                return;
            }
        }
    }
    else
    {
        cout << "ERROR 3 or 4 Arguments must be given" << endl;
        cout << "ERROR Total arguments given was: " << length - 1 << endl;
        return;
    }
}

void vaccineMonitor::insertCitizenRecord(string line)
{
    cout << "Selected: /insertCitizenRecord" << endl;
    line.erase(0, 21);

    int length;
    string *words = splitString(line, &length);

    citizenRecord *citizen;

    if (this->checkSyntaxRecord("SYNTAX ERROR: ", length, words, line)) // the record had syntax errors
    {
        return;
    }

    addNewVirus(words[5]);
    linkedListStringNode *virus = virusList->search(words[5]);
    addNewCountry(words[3]);
    linkedListStringNode *country = countryList->search(words[3]);

    char status = 'n';
    string date = "";
    if (words[6].compare("YES") == 0)
    {
        status = 'y';
        date = words[7];
    }

    citizen = new citizenRecord(stoi(words[0]), words[1], words[2], country, stoi(words[4]), virus, status, date);

    string result = "";
    citizenRecord *alreadyInTree = NULL;

    tree = tree->insert(tree, citizen, &alreadyInTree, &result, true); // insert in tree
    // cout << "RESULT=" << result << endl;
    // if (alreadyInTree)
    // {
    //     cout << "ALREADY IN TREE=";
    //     alreadyInTree->print();
    //     cout << endl;
    // }
    if (result.compare("NEW CITIZEN") == 0)
    {
        bloomList->getBloom(virus)->add(citizen->getID());
        if (status == 'y')
        {
            skiplist->getVaccinated(virus)->add(citizen->getID(), citizen);
        }
        else if (status == 'n')
        {
            skiplist->getNotVaccinated(virus)->add(citizen->getID(), citizen);
        }
        cout << "CITIZEN " << words[0] << " ADDED" << endl;
    }
    else if (result.compare("WRONG CREDENTIALS") == 0)
    {
        cout << "ERROR: CITIZEN ID " << words[0] << " HAS CREDENTIALS";
        cout << " Firstname=" << alreadyInTree->getFirstName();
        cout << " Lastname=" << alreadyInTree->getLastName();
        cout << " Country=" << alreadyInTree->getCountry()->getString();
        cout << " Age=" << alreadyInTree->getAge();
        cout << endl;
        cout << "ERROR= " << result << endl;
    }
    else if (result.compare("VIRUS ADDED TO CITIZEN") == 0)
    {
        if (status == 'y')
        {
            bloomList->getBloom(virus)->add(alreadyInTree->getID());
            skiplist->getVaccinated(virus)->add(alreadyInTree->getID(), alreadyInTree);
        }
        else if (status == 'n')
        {
            skiplist->getNotVaccinated(virus)->add(alreadyInTree->getID(), alreadyInTree);
        }
        cout << "THE NEW VIRUS INFORMATION ADDED TO CITIZEN " << words[0] << endl;
    }
    else if (result.compare("OLD NO NEW NO") == 0)
    {
        cout << "ERROR: CITIZEN " << words[0] << " HAD ALREADY THAT INFORMATION ABOUT THAT VIRUS" << endl;
    }
    else if (result.compare("OLD NO NEW YES") == 0)
    {

        bloomList->getBloom(virus)->add(alreadyInTree->getID());
        // skiplist->getVaccinated(virus)->print();
        // cout << endl;
        // skiplist->getNotVaccinated(virus)->print();
        // cout << endl;
        skiplist->getVaccinated(virus)->add(alreadyInTree->getID(), alreadyInTree);
        skiplist->getNotVaccinated(virus)->remove(alreadyInTree->getID());
        // cout << endl;
        // skiplist->getVaccinated(virus)->print();
        // cout << endl;
        // skiplist->getNotVaccinated(virus)->print();

        cout << "CITIZEN " << words[0] << " STATUS CHANGED FROM [" << virus->getString() << " NO] TO [" << virus->getString() << " YES " << date << "]" << endl;
    }
    else if (result.compare("OLD YES NEW NO") == 0)
    {
        cout << "ERROR: CITIZEN " << words[0] << " HAS MADE THE VACCINE FOR " << virus->getString() << " ON ";
        alreadyInTree->getStatus()->getVirusDate(virus).print();
        cout << endl;
    }
    else if (result.compare("OLD YES NEW YES") == 0)
    {
        cout << "ERROR: CITIZEN " << words[0] << " ALREADY VACCINATED ON ";
        alreadyInTree->getStatus()->getVirusDate(virus).print();
        cout << endl;
    }
    delete[] words;
}

void vaccineMonitor::vaccinateNow(string *arguments, int length)
{
    cout << "Selected: /vaccinateNow" << endl;
}

void vaccineMonitor::listNonVaccinatedPersons(string *arguments, int length)
{
    cout << "Selected: /list-nonVaccinated-Persons" << endl;
}

void vaccineMonitor::teminate()
{
    cout << "Selected: exit" << endl;
}
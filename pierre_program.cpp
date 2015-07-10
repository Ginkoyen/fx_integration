/*
Programme pour Pierre RADAS qui :

1 - Lit un fichier .csv

2 - Rentre les donnees du fichier dans un tableau multi-dimenssionel

- Tableau constitue de 5 colonnes, exemple avec un echantillon de valeur :

Date | open_value | High_value | Low_value | close_value
1.51 | 581.887    | 24.4774    | 2.85285   | 477.858
2.80 | 847.4477   | 80.58      | 8.447     | 895.4774

3 - Comparee les x lignes et les rentres dans une ligne d un tableau resultat constitue de 5 colonnes du meme nom :
- Un fichier .csv constitue de 100 lignes.

- Date : La date la plus recente parmis les x lignes du tableau d entree est rentree dans la colonne date du tableau resultat
- open_value : La plus ancienne valeur de la colonne open_value parmis les x lignes du tableau d entree est rentree dans la colonne open_value du tableau resultat
- High_value : La plus haute valeur de la colonne High_Value parmis les x lignes du tableau d entree est rentree dans la colonne High_value du tableau resultat
- Low_value :  La plus basse valeur de la colonne Low_Value parmis les x lignes du tableau d entree est rentree dans la colonne Low_value du tableau resultat
- close_value : La plus recente valeur de la colonne close_value parmis les x lignes du tableau d entree est rentree dans la colonne close_value du tableau resultat

4 - Reiterer en commancant une ligne plus bas, toujours sur x lignes.

- L utilisateur choisi de comparer 25 lignes on commence a partir de la ligne 0.
#include <fstream>
- Les lignes 0 a 25 sont comparee, la ligne de resultat est rentree dans la premiere ligne du tableau resultat.
- Les lignes 1 a 26 sont comparee, la ligne de resultat est rentree dans la seconde ligne du tableau resultat.
- etc ...
*/
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

float converter_str_f(std::string value_str)
{
  float value_f;
  std::stringstream converter;
  converter << value_str;
  converter >> value_f;
  return value_f;
}

std::string converter_f_str(float value_f)
{
  std::string value_str;
  std::stringstream converter;
  converter << value_f;
  converter >> value_str;
  return value_str;
}

int countFileLine(std::string nameFile) // Count number line in a file
{
  int numberLine = 0;
  std::string ghostLine("");
  std::ifstream openFile(nameFile.c_str());

  if(!openFile)
    std::cout << "Error ! Unable to read the file for count the line" << std::endl;

  while(getline(openFile, ghostLine))
    numberLine++;

  openFile.close();
  return numberLine;
}

// function which read a line and write date, open, close in a columns of cache array
void parseLine(std::string& line, int lineIndex, float cache[][4], std::string dateCache[])
{
  std::string comma(",");

  std::string::size_type pos1 = line.find(comma);

  // full date value in array date
  std::string value_str = line.substr(0, pos1);
  dateCache[lineIndex] = value_str;

  // convert and full open, close... value in array float
  std::string::size_type pos2 = line.find(comma, pos1 + 1);
  std::string::size_type length = pos2 - pos1;

  value_str = line.substr(pos1 + 1, length - 1);
  float value_f = converter_str_f(value_str);
  cache[lineIndex][0] = value_f;

  pos1 = line.find(comma, pos2 + 1);
  length = pos1 - pos2;

  value_str = line.substr(pos2 + 1, length - 1);
  value_f = converter_str_f(value_str);
  cache[lineIndex][1] = value_f;

  pos2 = line.find(comma, pos1 + 1);
  length = pos2 - pos1;

  value_str = line.substr(pos1 + 1, length - 1);
  value_f = converter_str_f(value_str);
  cache[lineIndex][2] = value_f;

  value_str = line.substr(pos2 + 1);
  value_f = converter_str_f(value_str);
  cache[lineIndex][3] = value_f;
}

void integrate(int lineIndex, int numberLineCompare, int size_cache, float cache[][4], std::string dateCache[], std::string resultArray[][5])
{
  // Compute max of maxs .... and min of mins
  float maxOfHightValues =  cache[lineIndex % size_cache][1];
  float minOfLowValues = cache[lineIndex % size_cache][2];

  for(int i = lineIndex + 1; i < (lineIndex + numberLineCompare)-1; i++)
  {
    int cache_index = i % size_cache;
    if (cache[cache_index][1] > maxOfHightValues)
      maxOfHightValues = cache[cache_index][1];
    if (cache[cache_index][2] < minOfLowValues)
        minOfLowValues = cache[cache_index][2];
  }
  // copy the most recent date
  resultArray[lineIndex][0] = dateCache[lineIndex % size_cache];
  // copy older open_value among numberLineCompare
  resultArray[lineIndex][1] = converter_f_str(cache[(lineIndex + numberLineCompare)% size_cache][0]);
  // copy newest close_value among numberLineCompare
  resultArray[lineIndex][4] = converter_f_str(cache[lineIndex % size_cache][3]);

  resultArray[lineIndex][2] = converter_f_str(maxOfHightValues);

  resultArray[lineIndex][3] = converter_f_str(minOfLowValues);
}

// Function which process a CSV file and copy results in resultArray
bool processCSVFile(std::string& nameFile, int numberLineCompare, std::string resultArray[][5])
{
  std::ifstream openFile(nameFile.c_str());
  std::string line("");
  if(!openFile)
  {
    std::cout << "Error ! Unable to read " << nameFile << " file" << std::endl;
    return -1;
  }

  // create a cache with 2 times the size of lines to process for each iteration
  // in order to maximize reuse of line already read.
  int size_cache = numberLineCompare * 2;

  // Create a dinamic array (dateCache)
  std::string* dateCache;
  dateCache = new std::string[size_cache];

  if(dateCache == nullptr)
  {
    std::cout << "Error ! Unable to create date cache Array" << std::endl;
    return -1;
  }

  // Create a multi-dimensional dynamic array (cache)
  float (*cache)[4];
  cache = new float[size_cache][4];

  if(cache == nullptr)
  {
    std::cout << "Error ! Unable to create cache Array" << std::endl;
    return -1;
  }

  // initialize cache
  for(int i = 0; i < numberLineCompare-1; i++)
  {
    getline(openFile, line);
    parseLine(line, i % size_cache, cache, dateCache);
  }

  int lineIndex = 0;
  while(getline(openFile, line)) // Compare the x line for copy result in array
  {
    integrate(lineIndex, numberLineCompare, size_cache, cache, dateCache, resultArray);
    parseLine(line, (lineIndex + numberLineCompare) % size_cache, cache, dateCache);
    lineIndex++;
  }
  delete[] cache;
  delete[] dateCache;
  return true;
}


bool writeResultArray(std::string resultArray[][5], int size_resultArray) // Function which read result array in a CSV file
{
  std::string createFile("resultArray.csv");
  std::ofstream writeFile(createFile.c_str());

  if(!writeFile)
  {
    std::cout << "Error ! Unable to create resultArray.csv file" << std::endl;
    return false;
  }
  std::cout << size_resultArray - 1 << std::endl;
  for(int i = 0; i <= size_resultArray - 1; i++)
  {
    writeFile << resultArray[i][0];
    writeFile << ",";
    writeFile << resultArray[i][1];
    writeFile << ",";
    writeFile << resultArray[i][2];
    writeFile << ",";
    writeFile << resultArray[i][3];
    writeFile << ",";
    writeFile << resultArray[i][4];
    writeFile << std::endl;
  }
  return true;
}

int main(int argc, char **argv)
{
  std::string nameFile("fx_base.csv");
  int numberLineCompare = 25; // Number of line to compare chosen by the user
  int numberLine_baseFile = 0;

//  std::cout << "Hello, enter file name : ";
//  std::cin >> nameFile;
//  std::cout << "Enter number line to compare : ";
//  std::cin >> numberLineCompare;

  numberLine_baseFile = countFileLine(nameFile); // count number line in fx_base file

  std::string (*resultArray)[5]; // Create a multi-dimensional array (resultArray)
  resultArray = new std::string[numberLine_baseFile][5]();
  if(resultArray == nullptr)
  {
    std::cout << "Error ! Unable to create result Array" << std::endl;
    return -1;
  }

  if( ! processCSVFile(nameFile, numberLineCompare, resultArray)) return -1;
  if( ! writeResultArray(resultArray, numberLine_baseFile)) return -1;
  std::cout << "Done !" << std::endl;
  return 0;
}

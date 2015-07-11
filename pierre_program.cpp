/*
Programme pour Pierre RADAS qui :

1 - Lit un fichier .csv

2 - Rentre les donnees du fichier dans un tableau multi-dimenssionel

- Tableau constitue de 5 colonnes, exemple avec un echantillon de valeur :

Date | open_value | Hight_value | Low_value | close_value
1.51 | 581.887    | 24.4774    | 2.85285   | 477.858
2.80 | 847.4477   | 80.58      | 8.447     | 895.4774

3 - Comparee les x lignes et les rentres dans une ligne d un tableau resultat constitue de 5 colonnes du meme nom :
- Un fichier .csv constitue de 100 lignes.

- Date : La date la plus recente parmis les x lignes du tableau d entree est rentree dans la colonne date du tableau resultat
- open_value : La plus ancienne valeur de la colonne open_value parmis les x lignes du tableau d entree est rentree dans la colonne open_value du tableau resultat
- Hight_value : La plus haute valeur de la colonne Hight_Value parmis les x lignes du tableau d entree est rentree dans la colonne Hight_value du tableau resultat
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
#include <sstream>
#include <fstream>

float converter_str_f(std::string value_str)// convert a str type to a float type
{
  float value_f;
  std::stringstream converter;
  converter << value_str;
  converter >> value_f;
  return value_f;
}

int converter_str_int(std::string value_str)// convert a str type to a int type
{
  int value_int;
  std::stringstream converter;
  converter << value_str;
  converter >> value_int;
  return value_int;
}


// function which read a line of CSV file and write open_value, close_value... in a columns of
// cache array and the date in date cache array
void parseLine(std::string& line, int lineIndex, float cache[][4], std::string dateCache[])
{
  std::string comma(",");
  std::string::size_type pos1 = line.find(comma);// Position to the first comma

  // full date value in array date
  std::string value_str = line.substr(0, pos1);
  dateCache[lineIndex] = value_str;// Bring date in date cache array

  // convert and full open, close... value in array float
  std::string::size_type pos2 = line.find(comma, pos1 + 1);// Position to the second comma
  std::string::size_type length = pos2 - pos1;// length beetwen the first comma to the second comma

  value_str = line.substr(pos1 + 1, length - 1);
  float value_f = converter_str_f(value_str);
  cache[lineIndex][0] = value_f;// Bring open_value in cache array

  pos1 = line.find(comma, pos2 + 1);// Position to the third comma
  length = pos1 - pos2;// length beetwen the second comma to the third comma

  value_str = line.substr(pos2 + 1, length - 1);
  value_f = converter_str_f(value_str);
  cache[lineIndex][1] = value_f;// Bring hight_value in cache array

  pos2 = line.find(comma, pos1 + 1);// Position to the fourth comma
  length = pos2 - pos1;// length beetwen the fird comma to the fourth comma

  value_str = line.substr(pos1 + 1, length - 1);
  value_f = converter_str_f(value_str);
  cache[lineIndex][2] = value_f;// Bring low_value in cache array

  value_str = line.substr(pos2 + 1);
  value_f = converter_str_f(value_str);
  cache[lineIndex][3] = value_f;// Bring close_value in cache array
}


// compare number of line chosen by user for bring the hight_value, low_value... in the result array
void integrate(int lineIndex, int numberLineCompare, int size_cache, float cache[][4], std::string dateCache[], std::ofstream& writeFile)
{
        // ## Compute max of maxs .... and min of mins ## //

   float maxOfHightValues =  cache[lineIndex % size_cache][1];
   float minOfLowValues = cache[lineIndex % size_cache][2];

   for(int i = lineIndex + 1; i < lineIndex + numberLineCompare; i++)
   {
     int cache_index = i % size_cache;

     if(cache[cache_index][1] > maxOfHightValues)
       maxOfHightValues = cache[cache_index][1];

     if(cache[cache_index][2] < minOfLowValues)
       minOfLowValues = cache[cache_index][2];
   }

               // ## Write result in csv file ## //

   // write the most recent date in result csv file
   writeFile << dateCache[lineIndex % size_cache] << ",";
   // write older open_value among numberLineCompare in result csv file
   writeFile << cache[(lineIndex + numberLineCompare - 1)% size_cache][0] << ",";
   // write newest close_value among numberLineCompare in result csv file
   writeFile << cache[lineIndex % size_cache][3] << ",";
   // write hight_value among numberLineCompare in result csv file
   writeFile << maxOfHightValues << ",";
   // write low_value among numberLineCompare in result csv file
   writeFile << minOfLowValues << std::endl;
}


// Function which call parseLine function which bring line to CSV file in a cache and
// integrate function which compare number of line chosen by user for bring the hight_value,
// low_value... in the result result in csv file
bool processCSVFile(std::string& nameFile, int numberLineCompare)
{
               //  ## Open and Create CSV files ## //

  // Open the CSV file which contain the donnees
  std::ifstream openFile(nameFile.c_str());
  std::string line("");
  if(!openFile)
  {
    std::cout << "Error ! Unable to read " << nameFile << " file" << std::endl;
    return -1;
  }
  // Create and open the result csv file
  std::string createFile("fx_result.csv");
  std::ofstream writeFile(createFile.c_str());
  if(!writeFile)
  {
    std::cout << "Error ! Unable to create resultArray.csv file" << std::endl;
    return false;
  }
                     // ## Create caches ## //

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
  for(int i = 0; i < numberLineCompare; i++)
  {
    getline(openFile, line);
    parseLine(line, i % size_cache, cache, dateCache);
  }

        // ## Compare line, copy line in caches and write result ## //

  // Compare the x line, write result in csv file and copy line base csv file in caches
  // Repeat as long as there is a line in base csv file
  int lineIndex = 0;
  while(getline(openFile, line))
  {
    integrate(lineIndex, numberLineCompare, size_cache, cache, dateCache, writeFile);
    parseLine(line, (lineIndex + numberLineCompare) % size_cache, cache, dateCache);
    lineIndex++;
  }
  // Compare the last line and write result in csv file
  integrate(lineIndex, numberLineCompare, size_cache, cache, dateCache, writeFile);

  // Destructors
  openFile.close();
  writeFile.close();
  delete[] cache;
  delete[] dateCache;
  return true;
}


// Function which read config file for find csv name File and number line to compare
bool readConfigFile(std::string& nameFile, int& numberLineCompare)
{
   std::string line("");
   std::string configFileName("config.txt");// load configuration file
   std::ifstream openFile(configFileName.c_str()); //Open for reading machineConfig file
   if(!openFile)
   {
     std::cout << "Error ! Unable read the configuration file" << std::endl;
     return false;
   }

   while(getline(openFile, line))// Read name csv file and number of line to compare in config file
   {
      if(line.find("NAME_BASE_CSV_FILE") != std::string::npos)
      {
         int pos = line.find(" ");
         nameFile = line.substr(pos+1);
      }
      else if(line.find("NUMBER_OF_LINE_TO_COMPARE") != std::string::npos)
      {
         int pos = line.find(" ");
         numberLineCompare = converter_str_int(line.substr(pos+1));
      }
   }
   return true;
}


int main(int argc, char **argv)
{
  std::string nameFile("");   // name CSV file which contain the donnees
  int numberLineCompare = 0; // Number of line to compare chosen by the user

  // read config file for find csv name File and number line to compare
  if(!readConfigFile(nameFile, numberLineCompare)) return -1;

  // Bring line to CSV file in a cache.
  // compare number of line chosen by user for bring the hight_value, low_value... in the result array
  // write result array in CSV file
  if(!processCSVFile(nameFile, numberLineCompare)) return -1;
  std::cout << "Done !" << std::endl;
  return 0;
}

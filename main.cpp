#include <bits/stdc++.h>
using namespace std;

// node for huffman tree
struct Node
{
   char character;
   int frequency;
   struct Node *left, *right;
   Node(char c, int f) : character(c), frequency(f), left(nullptr), right(nullptr) {}
   bool operator<(const Node &other) const
   {
      return frequency > other.frequency;
   }
};

// function to construct a minimum heap with given data
priority_queue<Node> buildMinHeap(vector<char> &chr, vector<int> &freq)
{
   priority_queue<Node> min_Heap;
   for (int i = 0; i < chr.size(); ++i)
   {
      Node *node = new Node(chr[i], freq[i]);
      min_Heap.push(*node);
   }
   return min_Heap;
}

// function to build huffman tree
Node *buildHuffman(priority_queue<Node> minHeap)
{
   while (minHeap.size() > 1)
   {
      Node *left = new Node(minHeap.top());
      minHeap.pop();
      Node *right = new Node(minHeap.top());
      minHeap.pop();

      Node *mergedNode = new Node('$', left->frequency + right->frequency);
      mergedNode->left = left;
      mergedNode->right = right;
      minHeap.push(*mergedNode);
   }
   return new Node(minHeap.top());
}

// function to print huffman table
unordered_map<char, string> huffmanCodes;
unordered_map<char, string> HuffmanCodes(Node *root, string code = "")
{
   if (root == nullptr)
   {
      return huffmanCodes;
   }
   if (root->character != '$')
   {
      huffmanCodes[root->character] = code;
   }
   HuffmanCodes(root->left, code + "0");
   HuffmanCodes(root->right, code + "1");
   return huffmanCodes;
}

// function to change charecters to huffman code
void compressor(string f_name, const std::string &outputFileName)
{
   fstream inp_file;
   inp_file.open(f_name, ios::in);
   std::ofstream outputFile(outputFileName, std::ios::binary);
   if (!outputFile.is_open())
   {
      std::cerr << "Error opening output file!" << std::endl;
      return;
   }

   // write the size of the huffman codes map to the output file
   size_t huffmanCodesSize = huffmanCodes.size();
   outputFile.write(reinterpret_cast<const char *>(&huffmanCodesSize), sizeof(size_t));

   // print the huffman codes on output.txt
   for (const auto &entry : huffmanCodes)
   {
      outputFile.put(entry.first);
      outputFile.put(entry.second.length());
      outputFile << entry.second;
   }

   // separator between Huffman codes and encoded text
   outputFile.put('\n');

   // encoded text to output.txt using bit manipulation
   string encodedBits;
   string text;
   while (inp_file)
   {
      getline(inp_file, text);
      for (char ch : text)
      {
         encodedBits += huffmanCodes.at(ch);
      }
   }

   // calcilate number of bits to be added
   size_t padding = 8 - (encodedBits.length() % 8);
   encodedBits += std::string(padding, '0');

   // print size of the encoded text to output.txt
   size_t encodedBitsSize = encodedBits.size();
   outputFile.write(reinterpret_cast<const char *>(&encodedBitsSize), sizeof(size_t));

   // encoded text to output.txt in bytes
   for (size_t i = 0; i < encodedBitsSize; i += 8)
   {
      std::bitset<8> bits(encodedBits.substr(i, 8));
      outputFile.put(bits.to_ulong());
   }

   outputFile.close();

   std::cout << "File compressed successfully." << std::endl;
}

// Decompress
// function to decompress output.txt
void decompressor(string &inputFileName)
{
   // input file read type
   std::ifstream inputFile(inputFileName, std::ios::binary);
   // if file exists or not
   if (!inputFile.is_open())
   {
      std::cerr << "Error opening input file!" << std::endl;
      return;
   }

   // read the size of the huffman codes map from the input file
   size_t huffmanCodesSize;
   inputFile.read(reinterpret_cast<char *>(&huffmanCodesSize), sizeof(size_t));

   // read the huffman codes from the input file
   std::unordered_map<char, std::string> huffmanCodes;
   for (size_t i = 0; i < huffmanCodesSize; ++i)
   {
      char character;
      inputFile.get(character);

      char codeLength;
      inputFile.get(codeLength);

      std::string code;
      for (int j = 0; j < codeLength; ++j)
      {
         code += inputFile.get();
      }

      huffmanCodes[character] = code;
   }

   // read separator between huffman codes and encoded text
   inputFile.get();

   // read the size of the encoded text from input.txt
   size_t encodedBitsSize;
   inputFile.read(reinterpret_cast<char *>(&encodedBitsSize), sizeof(size_t));

   // Read the encoded text from input.txt in bytes
   std::string encodedBits;
   char byte;
   while (inputFile.get(byte))
   {
      encodedBits += std::bitset<8>(byte).to_string();
   }

   // reduce to the actual size
   encodedBits.resize(encodedBitsSize);

   // close the input file
   inputFile.close();

   // decoding
   std::string decodedText;
   std::string currentCode;

   for (char bit : encodedBits)
   {
      currentCode += bit;
      for (const auto &entry : huffmanCodes)
      {
         if (entry.second == currentCode)
         {
            decodedText += entry.first;
            currentCode.clear();
            break;
         }
      }
   }
   fstream out_file;
   out_file.open("decompressed.txt", ios::out);
   out_file << decodedText;
   cout << "File decompressed successfully." << endl;
}

// store unique characters in vector
vector<char> buildChar(unordered_map<char, int> map)
{
   vector<char> unique_chars;
   for (auto pair : map)
   {
      unique_chars.push_back(pair.first);
   }
   return unique_chars;
}

// store frequencies of unique characters in another vector
vector<int> buildFreq(unordered_map<char, int> map)
{
   vector<int> unique_freq;
   for (auto pair : map)
   {
      unique_freq.push_back(pair.second);
   }
   return unique_freq;
}

// count the number of unique charecters and thir frequencies
unordered_map<char, int> countChar(string &file_name)
{
   unordered_map<char, int> charCount;
   fstream file;
   file.open(file_name, ios::in);
   if (!file)
   {
      cout << "check file name";
      return charCount;
   }
   while (file)
   {
      string line;
      getline(file, line);

      for (char c : line)
      {
         if (charCount.find(c) == charCount.end())
         {
            charCount[c] = 1;
         }
         else
         {
            charCount[c]++;
         }
      }
   }
   return charCount;
}

// MAIN
int main()
{
   int ch;
   string f_name;
   string file;

   cout << "____________________" << endl;
   cout << "|     OPTIONS      |" << endl;
   cout << "_________________" << endl;
   cout << "| 1.Compress    |" << endl;
   cout << "| 2.Decompress  |" << endl;
   cout << " " << endl;
   cout << "Enter choice :";
   cin >> ch;

   if (ch == 1)
   {
      cout << "Enter Filename :";
      cin >> f_name;

      // count unique characters and their frequencies
      unordered_map<char, int> result = countChar(f_name);

      // store unique characters and their frequencies in two separate arrays
      vector<char> uniqueChars = buildChar(result);
      vector<int> uniqueFreq = buildFreq(result);

      // sort the above two data with min-heap
      priority_queue<Node> minHeap = buildMinHeap(uniqueChars, uniqueFreq);

      // build huffman tree from minHeap
      Node *root = buildHuffman(minHeap);

      // generate huffman codes

      unordered_map<char, string> temp = HuffmanCodes(root, "");

      for (const auto &it : temp)
      {
         cout << it.first << " " << it.second << endl;
      }

      // substitute huffman codes(compressor)
      compressor(f_name, "output.txt");
   }
   if (ch == 2)
   {
      cout << "Enter filename: ";
      cin >> file;
      cout << "Decompressing the file..." << endl;
      // decompress
      decompressor(file);
   }
   return 0;
}
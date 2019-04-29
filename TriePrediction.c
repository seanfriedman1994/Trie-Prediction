//Sean Friedman
//Trie Prediction

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "TriePrediction.h"


//create a trieNode and properly initialize
TrieNode *createTrieNode(void)
{
	return calloc(1, sizeof(TrieNode));
}

//strips punctuation from a string
void stripPunctuators(char *str)
{
	if(str == NULL)
	{
		return;
	}
	
	int i, j = 0, newLen, len = strlen(str);
	
	char *newString = calloc(1, sizeof(char) * (len + 1));
	if(newString == NULL)
	{
		free(newString);
		return;
	}
	
	
	for(i = 0; i < len; i++)
	{
		//check if char is alphabetic, if so add to new string.
		if (isalpha(str[i]))
		{
			newString[j] = str[i];
			j++;
		}
	}
	
	//add the null terminator
	newString[j] = '\0';

	//copy the created string onto the passed in string, prevent memory leaks
	strcpy(str, newString);
	
	free(newString);
	
}

// Insert a string into a trie. This function returns the root of the trie. (Credit: Dr. S)
TrieNode *insertString(TrieNode *root, char *str)
{
	if(str == NULL)
	{
		return NULL;
	}
	
	int i, index;
	TrieNode *wizard;

	if (root == NULL)
	{
		root = createTrieNode();
	}
	

	stripPunctuators(str);
	
	wizard = root;
	
	for (i = 0; i < strlen(str); i++)
	{
		index = tolower(str[i]) - 'a';

		//check if node exists, create it if not
		if (wizard->children[index] == NULL)
			wizard->children[index] = createTrieNode();

		//jump the wizard forward
		wizard = wizard->children[index];
	}

		
	//increment the count of the wizard which is now at the
	//terminal node
	wizard->count++;
	return root;
}


// Helper function called by printTrie(). (Credit: Dr. S.)
void printTrieHelper(TrieNode *root, char *buffer, int k)
{
	int i;

	if (root == NULL)
		return;

	if (root->count > 0)
		printf("%s (%d)\n", buffer, root->count);

	buffer[k + 1] = '\0';

	for (i = 0; i < 26; i++)
	{
		buffer[k] = 'a' + i;

		printTrieHelper(root->children[i], buffer, k + 1);
	}

	buffer[k] = '\0';
}

// If printing a subtrie, the second parameter should be 1; otherwise, if
// printing the main trie, the second parameter should be 0. (Credit: Dr. S.)
void printTheTrie(TrieNode *root, int useSubtrieFormatting)
{
	char buffer[1026];

	if (useSubtrieFormatting)
	{
		strcpy(buffer, "- ");
		printTrieHelper(root, buffer, 2);
	}
	else
	{
		strcpy(buffer, "");
		printTrieHelper(root, buffer, 0);
	}
}


//build Trie function (Credit: Dr. S)
TrieNode *buildTrie(char *filename)
{
	int len, flag = 0;
	TrieNode *root = NULL, *termNode = NULL;
	char buffer[MAX_CHARACTERS_PER_WORD + 1];
	FILE *ifp;
	if ((ifp = fopen(filename, "r")) == NULL)
	{
		fprintf(stderr, "Failed to open \"%s\" in buildTrie().\n", filename);
		return NULL;
	}
	
	// Insert strings one-by-one into the trie.
	while (fscanf(ifp, "%s", buffer) != EOF)
	{
		len = strlen(buffer) - 1;
		
		//checks for punctuation at the end of a sentence.
		//will not add the following word into the current words subtrie.
		if((buffer[len] == '.') || (buffer[len] == '?') || (buffer[len] == '!'))
		{
			if(termNode != NULL)
			{
				termNode->subtrie = insertString(termNode->subtrie, buffer);
			}
			root = insertString(root, buffer);
			termNode = NULL;
		}
		//insert each string into the trie, and adds subtries to each string
		else if(root != NULL)
		{
			if(termNode != NULL)
			{
				termNode->subtrie = insertString(termNode->subtrie, buffer);	
			}
			root = insertString(root, buffer);
			termNode = getNode(root, buffer);
		}
		else
		{
			root = insertString(root, buffer);
			termNode = getNode(root, buffer);
		}
	}
	fclose(ifp);
	
	return root;
}

//processes the Input File and executes given commands
int processInputFile(TrieNode *root, char *filename)
{
	FILE *ifp;
	TrieNode *termNode = NULL;
	char buffer[MAX_CHARACTERS_PER_WORD + 5];
	
	int i, n = 0;
	
	if ((ifp = fopen(filename, "r")) == NULL)
	{
		return 1;
	}
	
	while(fscanf(ifp, "%s", buffer) != EOF)
	{
		//if the command is "!" then print the trie
		if(strcmp(buffer, "!") == 0)
		{
			printTheTrie(root, 0);
		}
		
		//if the command is the string by itself, then 
		//print the string and its subtrie contents
		if((strcmp(buffer, "!") != 0) && (buffer[0] != '@'))
		{
			printf("%s\n", buffer);
			
			if(getNode(root, buffer) == NULL)
			{
				printf("(INVALID STRING)\n");
			}else if(getNode(root, buffer)->subtrie == NULL)
			{
				printf("(EMPTY)\n");
			}else
			{
				printTheTrie(getNode(root, buffer)->subtrie, 1);
			}
		}
		
		//if the command begins with an "@" then predict the text
		//by finding the most frequent word in the given words subtrie
		if(strcmp(buffer, "@") == 0)
		{
			fscanf(ifp, "%s", buffer);
			fscanf(ifp, "%d", &n);
			
			printf("%s", buffer);
			termNode = root;
			for(i = 0; i < n; i++)
			{
				
				//get the terminal node of passed in string
				termNode = getNode(root, buffer);
				
				//breaks out of the loop if necessary, 
				//otherwise prints a space for the next word
				if((termNode == NULL) || (termNode->subtrie == NULL))
				{
					break;
				}else
				{
					printf(" ");
				}
				//find the most frequent word in the subtrie of 
				//terminal node
				getMostFrequentWord(termNode->subtrie, buffer);
				printf("%s", buffer);
			}
			
			printf("\n");
			
		}
	}
	
	fclose(ifp);
	
	return 0;
}

//recursively destroys the entire Trie
TrieNode *destroyTrie(TrieNode *root)
{
	int i;
	
	if(root == NULL)
	{
		return NULL;
	}
	
	//traverse the trie and recursively call the function
	for(i = 0; i < 26; i++)
	{
		if(root->children[i] != NULL)
		{
			root->children[i] = destroyTrie(root->children[i]);
		}
	}
	
	//traverse any and all subtries and recursively call the function
	if(root->subtrie != NULL)
	{
		root->subtrie = destroyTrie(root->subtrie);
	}	
	
	free(root);	
	
	return NULL;
}

//takes a given string and finds it in the trie.
//if found, returns the terminal node.
TrieNode *getNode(TrieNode *root, char *str)
{
	int i, letter;
	
	if((root == NULL) || (str == NULL))
	{
		return NULL;
	}
	
	char *newStr = calloc(1, sizeof(char) * (MAX_CHARACTERS_PER_WORD + 1));
	strcpy(newStr, str);
	
	stripPunctuators(newStr);
	
	//make the string lowercase
	for(i = 0; i < strlen(newStr); i++)
	{
		newStr[i] = tolower(newStr[i]);
	}

	//traverse the trie with the created string
	for(i = 0; i < strlen(newStr); i++)
	{
		letter = newStr[i] - 'a';
		if(root->children[letter] != NULL)
		{
			root = root->children[letter];
		}
		else
		{
			free(newStr);
			return NULL;
		}
	}
	
	//check if the string is represented in the trie
	//return the terminal node if so
	if(root->count > 0)
	{
		free(newStr);
		return root;
	}
	
	free(newStr);
	return NULL;
}

//helper function for getMostFrequentWord, recursively traverses the trie
//and returns whichever string is most frequent
int recursiveFrequentWord(TrieNode *root, char *str, char *newStr, int mostFrequentCount, int strIndex)
{
	int i, flag;
	char letter;
	
	if((root == NULL) || (str == NULL))
	{
		return 0;
	}
	
	//copies the string into the passed string only if 
	//its count is the highest seen so far
	if(root->count > mostFrequentCount)
	{
		mostFrequentCount = root->count;
		strcpy(str, newStr);
	}
	
	for(i = 0; i < 26; i++)
	{
		//appends each found letter onto a new String.
		if(root->children[i] != NULL)
		{
				
			letter = i + 'a';
			newStr[strIndex] = letter;
			strIndex++;
			newStr[strIndex] = '\0';
			//recursively calls the function and returns the highest count found to locate the proper string
			mostFrequentCount = recursiveFrequentWord(root->children[i], str, newStr, mostFrequentCount, strIndex);
			//decrement the index and add a null terminator each time in order to clear the string
			strIndex--;
			newStr[strIndex] = '\0';
		}
	}	
	
	return mostFrequentCount;
}

//finds the most frequent word in the trie
void getMostFrequentWord(TrieNode *root, char *str)
{
	int i, mostFrequentCount = 0, strIndex = 0;
	char *newStr = calloc(1, sizeof(char) * (MAX_CHARACTERS_PER_WORD + 1));
	
	if((root == NULL) || (str == NULL))
	{
		strcpy(str, "");
		free(newStr);
		return;
	}
	
	mostFrequentCount = recursiveFrequentWord(root, str, newStr, mostFrequentCount, strIndex);
	
	free(newStr);
}

//checks if the string passed is in the trie
int containsWord(TrieNode *root, char *str)
{
	int i, letter;
	char *newStr = calloc(1, sizeof(char) * (MAX_CHARACTERS_PER_WORD + 1));
	
	if((root == NULL) || (str == NULL))
	{
		free(newStr);
		return 0;
	}
	
	strcpy(newStr, str);
	
	//strip punctuation and change to lowercase if necessary
	stripPunctuators(newStr);
	
	for(i = 0; i < strlen(newStr); i++)
	{
		newStr[i] = tolower(newStr[i]);
	}
	
	//traverse the trie to find the string
	for(i = 0; i < strlen(newStr); i++)
	{
			letter = newStr[i] - 'a';
			if(root->children[letter] != NULL)
			{
				root = root->children[letter];
			}
			else
			{
				free(newStr);
				return 0;
			}
	}
	
	//check if the string is represented in the trie
	//return 1 if so, 0 otherwise
	if(root->count > 0)
	{
		free(newStr);
		return 1;
	}
	
	free(newStr);
	return 0;
}

//recursively traverse the trie and add up any words
//starting at given prefix
int recursiveWordCount(TrieNode *root, int stringCount)
{
	int i;
	
	if(root == NULL)
	{
		return 0;
	}
	
	//if the node's count is higher than 0, add the
	//value to the counter as it represents a string.
	if(root->count > 0)
	{
		stringCount += root->count;
	}
	
	//traverse the trie and recursively call the function
	for(i = 0; i < 26; i++)
	{
		if(root->children[i] != NULL)
		{
			stringCount = recursiveWordCount(root->children[i], stringCount);
		}
	}
	
	return stringCount;
}

//find any words in the trie starting with and 
//including the prefix
int prefixCount(TrieNode *root, char *str)
{
	int i, letter, stringCount = 0;
	char *newStr = calloc(1, sizeof(char) * (MAX_CHARACTERS_PER_WORD + 1));
	
	if((root == NULL) || (str == NULL))
	{
		free(newStr);
		return 0;
	}
	
	strcpy(newStr, str);
	
	//check for empty string, no need to traverse the trie
	//or play with the string if so
	if(strcmp(newStr, "") == 0)
	{
		stringCount = recursiveWordCount(root, stringCount);
		
		free(newStr);
		return stringCount;
	}
	
	//strip punctuation, make lowercase if necessary
	stripPunctuators(newStr);
	
	for(i = 0; i < strlen(newStr); i++)
	{
		newStr[i] = tolower(newStr[i]);
	}
	
	//traverse the trie to get to the terminal node 
	//of given prefix, return 0 if it doesn't exist
	for(i = 0; i < strlen(newStr); i++)
	{
		letter = newStr[i] - 'a';
		if(root->children[letter] != NULL)
		{
			root = root->children[letter];
		}
		else
		{
			free(newStr);
			return 0;
		}
	}
	
	//pass in the terminal node to the recursiveWordCount function
	stringCount = stringCount + recursiveWordCount(root, stringCount);
	
	free(newStr);
	
	return stringCount;
}

double difficultyRating(void)
{
	return 4.4;
}

double hoursSpent(void)
{
	return 25.0;
}


int main(int argc, char **argv)
{
	TrieNode *root = buildTrie(argv[1]);
	
	processInputFile(root, argv[2]);

	root = destroyTrie(root);
	
	return 0;
}


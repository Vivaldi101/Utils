#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define internal static
#define true 1
#define false 0
#define NUM_ALPHAS 128

typedef int bool32;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef int s32;
typedef unsigned char byte;

typedef struct trie_node_t
{
    struct trie_node_t *alpha[NUM_ALPHAS];
    const char *key;
    int is_external;
    int num_keys;
} trie_node_t;

internal trie_node_t *
GetTrieNode()
{
    trie_node_t *result = (trie_node_t *)malloc(sizeof(trie_node_t));
    memset(result, 0, sizeof(*result));
    result->num_keys = 1;

    return result;
}

internal void
InsertTrieNode(trie_node_t *node, const char *string, int pos)
{
    // Reached the end of the string.
    // Try to insert a node with the string.
    if(!string[pos])
    {
        if(node->alpha[string[pos]])
        {
            node->alpha[string[pos]]->num_keys++;
        }
        else
        {
            node->alpha[string[pos]] = GetTrieNode();
            node->alpha[string[pos]]->is_external = true;
            node->alpha[string[pos]]->key = string;
        }
        return;
    }
    // If while traversing the trie for the string, we find a node that does not have a node -- we mark it as non-leaf.
    if(!node->alpha[string[pos]])
    {
        node->alpha[string[pos]] = GetTrieNode();
        node->alpha[string[pos]]->is_external = false;
    }
    InsertTrieNode(node->alpha[string[pos]], string, pos + 1);
}

internal bool
SearchTrie(trie_node_t *node, const char *string, int pos)
{
    if(!node->alpha[string[pos]])
    {
        return false;
    }
    else if(!string[pos] && node->alpha[string[pos]]->is_external)
    {
        return true;
    }
    return SearchTrie(node->alpha[string[pos]], string, pos + 1);
}

internal void
PrintStrings(trie_node_t *node)
{
    int i;
    if(node)
    {
        // String is stored in the leaf.
        if(node->is_external)
        {
            for(i = 0; i < node->num_keys; i++)
            {
                printf(node->key);
                printf("\n");
            }
            return;
        }
        for(i = 0; i < NUM_ALPHAS; i++)
        {
            if(node->alpha[i])
            {
                PrintStrings(node->alpha[i]);
            }
        }
    }
}

internal void
PrintPrefixMatches(trie_node_t *node, const char *string, int pos)
{
    if(node)
    {
        if(!string[pos])
        {
            PrintStrings(node);
            return;
        }
        if(node->alpha[string[pos]])
        {
            PrintPrefixMatches(node->alpha[string[pos]], string, pos+1);
        }
    }
}

// Invariant: A valid node has the "string" as a prefix match if the "string"'s first character
// matches the character stored in the node.
// Otherwise keep looking depth first into the trie by scanning the rest of the characters stored in the child nodes of the upmost valid node.
internal void
PrintSubstringMatches(trie_node_t *node, const char *string, int index)
{
    if(node)
    {
        int i;
        for(i = 0; i < NUM_ALPHAS; i++)
        {
            // Start of a possible string match.
            if(node->alpha[i])
            {
                if(i == string[index]) // Match.
                {
                    PrintPrefixMatches(node, string, index);
                }
                else // Else keep searching through the rest of the trie.
                {
                    PrintSubstringMatches(node->alpha[i], string, index);
                }
            }
        }
    }
}

internal void
DeleteTrie(trie_node_t *node)
{
    int i;
    if(!node || node->is_external)
    {
        return;
    }
    for(i = 0; i < NUM_ALPHAS; i++)
    {
        if(node->alpha[i])
        {
            DeleteTrie(node->alpha[i]);
        }
    }
    free(node);
}

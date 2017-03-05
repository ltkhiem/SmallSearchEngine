#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <string>
#include <ctime>
#include <windows.h>
#include <iomanip>

#define NDATA 180
#define WORD_MAX_LENGTH 50
#define QUERY_MAX_WORD 50
#define N_STOP_WORD 174

using namespace std;

struct Node{
    char value;
    Node* pNext[62];
    bool incomplete[NDATA];
    bool complete[NDATA];
    vector <int> pos[NDATA];

    Node(char key);
};

struct Scoring
{
    int value=0;
    int docID;
};

struct charArr{
    char val[20];
};

class Trie{
    private:
        Node* pRoot=NULL;
        Node* pNil;

        bool MatchCase=false;
        bool TakeSnap=false;
        bool EliminateStopWord=false;

        bool wordIncomplete=false;
        int wordCount=0;
        vector <Node*> pSave;
        vector <Node*> pSaveIncomplete;

        bool AndRes[NDATA];
        bool OrRes[NDATA];
        Scoring ScoreRes[NDATA];

        int hsConsecutive = 1000000;
        int hsOrder = 1000;
        int hsOccurrence = 1;

        charArr StopWord[N_STOP_WORD];

        int numDoc=0;
        clock_t ProcessTime;

    public:
        FILE* fres;
        HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        Node* GetRoot();
        void InitTree();
        void LoadData(char path[], int DocID);
        void ProcessData();
        void Insert(char word[WORD_MAX_LENGTH], int docID, int wordCount);
        Node* Search(char word[]);
        void LoadQuery();
        void ProcessQuery(char word[WORD_MAX_LENGTH], int operate);
        void Output(Node* pCur, char arr[], int sl);
        void OutputQuery();
        void InitScore();
        void RankOccurrence();
        void RankConsecutiveAndNearest();
        void SortResult();
        void Free();

        int CalcNext(char s);
        void CToLower(char word[WORD_MAX_LENGTH]);
        void ProcessWord(char word[WORD_MAX_LENGTH], int docID);
        void OperAnd(bool Cur[NDATA]);
        void OperOr();
        void InitRes();
        void InitRes(bool Cur[NDATA]);
        void PreSnap(int docID);
        void Snapshot(int docID, int text[QUERY_MAX_WORD], int sl);
        void SetMatchCase(bool setting);
        void SetSnapshot(bool setting);
        void SetRank(int hs[3]);
        void SetDefaultRank();
        void SetDefaultProperties();
        void SetStopWord(bool setting);
        void LoadStopWord();
        bool CheckStopWord(char word[WORD_MAX_LENGTH]);
        int GetFullWord(Node* pCur, int docID);
        void FreeTree(Node* pCur);
};


#include "header.h"

Node::Node(char key)                //Init a new node with value key
{
    value = key;
    for (int i=0; i<62; i++)
        pNext[i] = NULL;
    for (int i=0; i<NDATA; i++)
    {
        complete[i]=false;
        incomplete[i]=false;
    }

}

void Trie::InitTree()                           //Init for a new tree
{
    pRoot = new Node('*');
    pNil = new Node('*');
    if (EliminateStopWord) LoadStopWord();
    fres=fopen("result.txt","w");
}

void Trie::SetMatchCase(bool setting)           //Setting for matching word's case
{
    MatchCase=setting;
}

void Trie::SetSnapshot(bool setting)            //Setting for displaying snapshot
{
    TakeSnap=setting;
}

void Trie::SetStopWord(bool setting)            //Setting for eliminating stop words
{
    EliminateStopWord=setting;
}

void Trie::SetRank(int hs[3])                   //Update the coefficient of ranking documents
{
    int score=1000000;
    for (int i=0; i<3; i++)
    {
        if (hs[i]==1)          hsConsecutive=score;
        else if (hs[i]==2)     hsOrder=score;
        else if (hs[i]==3)     hsOccurrence=score;
        else
        {
            SetDefaultRank();
            return;
        }
        score/=1000;
    }
}

void Trie::SetDefaultProperties()               //Setting up default properties
{
    MatchCase=false;
    TakeSnap=true;
}

void Trie::SetDefaultRank()                     //Setting up the default ranking coefficient
{
    hsConsecutive=1000000;
    hsOrder=1000;
    hsOccurrence=1;
}

void Trie::LoadStopWord()                       //Load the list of stop words
{
    int cnt=0;
    FILE *load;
    load=fopen("data/stopword.txt","r");
    if (load)
        while (fscanf(load,"%s",&StopWord[cnt].val)!=EOF)       cnt++;
    fclose(load);
}

bool Trie::CheckStopWord(char word[WORD_MAX_LENGTH])        //Check if a word is a stop word
{
    for (int i=0; i<N_STOP_WORD; i++)
        if (strcmp(word,StopWord[i].val)==0)    return true;
    return false;
}

int Trie::CalcNext(char s)                          //Calculate the pointer corresponding to the key s
{
    if ('0'<=s && s<='9') return(int(s)-48);
    if ('a'<=s && s<='z') return(int(s)-97+10);
    if ('A'<=s && s<='Z') return(int(s)-65+36);
}

void Trie::Insert(char word[WORD_MAX_LENGTH], int docID, int wordCount)     //Insert a word into the tree
{
    int j;  //direction to next node.
    Node* pCur=pRoot;   //Current node
    for (int i=0; i<strlen(word); i++)
    {
        j=CalcNext(word[i]);
        if (pCur->pNext[j] == NULL)
            pCur->pNext[j] = new Node(word[i]);
        pCur = pCur->pNext[j];
        pCur->incomplete[docID] = true;
        if (i==strlen(word)-1)
        {
            pCur->complete[docID] = true;               //  Save the ID of document
            pCur->pos[docID].push_back(wordCount);      // and position of word in document
        }
    }
}

Node* Trie::Search(char word[WORD_MAX_LENGTH])      //Search for a word in tree
{                                                   //Return pointer to a node in tree if found, otherwise return NULL
    Node* pCur = pRoot;
    for (int i=0; i<strlen(word); i++)
    {
        if (word[i]=='*')             //Check if keyword is a incomplete matching or not
        {
            wordIncomplete=true;
            return pCur;
        }
        int j=CalcNext(word[i]);
        if (pCur->pNext[j]!=NULL) pCur=pCur->pNext[j];
        else return NULL;
    }
    return pCur;
}

void Trie::CToLower(char word[WORD_MAX_LENGTH]) //Convert a word to lower case
{
    int c=0;
    for (int i=0; i<strlen(word); i++)
    {
        if ('A'<=word[i] && word[i]<='Z')
        {
            word[i] = char(int(word[i])+32);
        }
    }
}

void Trie::ProcessWord(char word[WORD_MAX_LENGTH], int docID)   //Some initial before inserting word
{
    wordCount++;
    if (!MatchCase)    CToLower(word);
    Insert(word,docID,wordCount);
}

void Trie::LoadData(char path[], int docID) //Load data from files
{
    char c,word[WORD_MAX_LENGTH];
    int len=0;
    strcpy(word,"");
    FILE *file;
    wordCount=0;
    file = fopen(path, "r");
    if (file)
    {
        while ((c = getc(file)) != EOF)
        {
            if ((65<=c && c<=90) || (97<=c && c<=122) || (48<=c && c<=57))
            {
                word[len]=char(c);
                len++;
                word[len]='\0';
            }
            else
            {
                if (strlen(word)>0) ProcessWord(word,docID);
                strcpy(word,"");
                len=0;
            }
        }
        if (strlen(word)>0) ProcessWord(word,docID);
        fclose(file);
    }
}

void Trie::OperAnd(bool Cur[NDATA])     //Process operation AND
{
    for (int i=0; i<NDATA; i++)
        AndRes[i] = AndRes[i] & Cur[i];
}

void Trie::OperOr()     //Process operation OR
{
    for (int i=0; i<NDATA; i++)
        OrRes[i] = OrRes[i] | AndRes[i];
}

void Trie::InitRes()    //Init result for query
{
    for (int i=0; i<NDATA; i++)
    {
        AndRes[i]=false;
        OrRes[i]=false;
    }
}

void Trie::InitRes(bool Cur[NDATA])
{
    for (int i=0; i<NDATA; i++)
        AndRes[i] = Cur[i];
}

void Trie::ProcessQuery(char word[WORD_MAX_LENGTH], int operate)  //Search for word and update result to query result
{
    Node* pRes;
    if (!MatchCase)    CToLower(word);
    pRes = Search(word);
    if (pRes==NULL)     pRes=pNil;
    else
        if (!wordIncomplete)    pSave.push_back(pRes);
        else    pSaveIncomplete.push_back(pRes);
    if (!wordIncomplete)
    {
        switch (operate)
        {
            case 0 :    InitRes(pRes->complete);    break;
            case 1 :    OperAnd(pRes->complete);    break;
            case 2 :    OperOr();
                        InitRes(pRes->complete);
                        break;
        }
    }
    else
    {
        switch (operate)
        {
            case 0 :    InitRes(pRes->incomplete);    break;
            case 1 :    OperAnd(pRes->incomplete);    break;
            case 2 :    OperOr();
                        InitRes(pRes->incomplete);
                        break;
        }
        wordIncomplete=false;
    }
}

void Trie::LoadQuery()  //Load query from file
{
    stringstream s;
    string s1;
    char word[WORD_MAX_LENGTH];
    int n, c, num, operate=0;       //operate=1 : And mode      operate=2: Or mode
    bool getOp=false;               //determine when we get Operator and when we get Word.
    FILE *file;
    ProcessTime = clock();          //get processing time

    if (TakeSnap)
    {
        cout << endl;
        SetConsoleTextAttribute(hConsole, 10);
        cout << "QUERY: ";
    }

    InitRes();
    file = fopen("data/query.txt", "r");
    if (file)
    {
        fscanf(file,"%d\n",&n);
        while ((c = getc(file)) != EOF)
        {
            if ((65<=c && c<=90) || (97<=c && c<=122) || (48<=c && c<=57) || (c==int('*'))) s<<char(c);
            else
            {
                s1 = s.str();
                strcpy(word,s1.c_str());
                if (TakeSnap) cout << word << " ";
                if (EliminateStopWord)
                    if (CheckStopWord(word))
                    {
                        continue;
                        getOp = !getOp;
                    }

                if (getOp)
                {
                    getOp = !getOp;
                    if (strcmp(word,"AND")==0)
                        operate = 1;
                    else if (strcmp(word,"OR")==0)
                        operate = 2;
                    else
                    {
                        if (strlen(word)>0) ProcessQuery(word,1);
                        getOp = !getOp;
                    }

                }
                else
                {
                    getOp=!getOp;
                    if (strlen(word)>0) ProcessQuery(word,operate);
                }
                s.str("");
            }
            if (c==int('\n'))
            {
                OperOr();
                SetConsoleTextAttribute(hConsole, 15);
                OutputQuery();
                operate=0;
                getOp=false;
                InitRes();
                if (TakeSnap)
                {
                    cout << endl;
                    cout << "-----------------------------------------------------------------------------" << endl;
                    SetConsoleTextAttribute(hConsole, 10);
                    cout << "QUERY: ";
                }

                ProcessTime=clock();    //Take processing time
            }
        }
        fclose(file);
    }
}

void Trie::Snapshot(int docID, int text[QUERY_MAX_WORD], int sl)
{
    bool blank=true;
    char c,word[WORD_MAX_LENGTH];
    stringstream s;
    string s1;
    FILE *file;
    int posnow=1;
    wordCount=0;

    s.str("");
    s<<"data/Data";
    if (docID<9) s<<"00";
    else if(docID<99) s<<"0";
    s<<docID+1<<".txt";
    s1 = s.str();
    strcpy(word,s1.c_str());

    file = fopen(word, "r");
    if (file)
    {
        s.str("");
        while ((c = getc(file)) != EOF)
        {
            if ((65<=c && c<=90) || (97<=c && c<=122) || (48<=c && c<=57)) s<<char(c);
            else
            {
                s1 = s.str();
                strcpy(word,s1.c_str());
                if (strlen(word)>0)
                {
                    wordCount++;
                    if (text[posnow]-6<wordCount && wordCount<text[posnow]+6)
                    {
                        if (wordCount==text[posnow])
                        {
                            SetConsoleTextAttribute(hConsole, 14);
                            cout << word << " ";
                            SetConsoleTextAttribute(hConsole, 15);
                        }

                        else
                            cout << word << " ";
                        blank=true;
                        if (wordCount==text[posnow]+5)  posnow++;
                        if (posnow>sl)
                        {
                            fclose(file);
                            return;
                        }
                    }
                    else
                    {
                        if (blank)
                        {
                            cout << "... ";
                            blank=false;
                        }
                    }
                }
                s.str("");
            }
        }
        fclose(file);
    }
}

int Trie::GetFullWord(Node* pCur, int docID)    //Get full word for incomplete matching
{
    int res;
    if (pCur->pos[docID].size()>0)
        return pCur->pos[docID][0];
    for (int i=0; i<62; i++)
        if (pCur->pNext[i]!=NULL)
        {
            res=GetFullWord(pCur->pNext[i],docID);
            if (res!=-1) return res;
        }
    return (-1);
}

void Trie::PreSnap(int docID)   //Take out position of words and sort in ascending order
{
    bool free=true;
    int text[QUERY_MAX_WORD];
    int sl=0, tmp;
    Node* pCur;
    for (int i=0; i<pSave.size(); i++)
    {
        pCur = pSave[i];
        if (pCur->pos[docID].size() > 0)
        {
            free=true;
            for (int j=1; j<=sl; j++)
                if (text[j]==pCur->pos[docID].at(0)) free=false;
            if (free)
            {
                sl++;
                text[sl]=pCur->pos[docID].at(0);
            }
        }
    }
    for (int i=0; i<pSaveIncomplete.size(); i++)
    {
        tmp=GetFullWord(pSaveIncomplete[i],docID);
        if (tmp!=-1)
        {
            sl++;
            text[sl]=tmp;
        }
    }
    sort(text+1,text+sl+1);
    Snapshot(docID,text,sl);
}

void Trie::OutputQuery()            //Output query to file and print snapshot
{
    InitScore();                    //
    RankOccurrence();                //  Ranks the documents
    RankConsecutiveAndNearest();    //  and sort them by score
    SortResult();                   //
    fprintf(fres,"%d ms ",clock()-ProcessTime); //Print processing time
    for (int i=0; i<numDoc; i++)
    {
        fprintf(fres,"Data");
        if (ScoreRes[i].docID<9) fprintf(fres,"00");
        else if (ScoreRes[i].docID<99) fprintf(fres,"0");
        fprintf(fres,"%d.txt ",ScoreRes[i].docID+1);
    }
    if (numDoc==0) fprintf(fres,"No data file matches the query");
    fprintf(fres,"\n");

    if (TakeSnap)
    {
        cout << endl << endl;
        int cntSnap=0;
        char c;
        for (int i=0; i<NDATA; i++)
        if (OrRes[i])
        {
            SetConsoleTextAttribute(hConsole, 11);
            cout << "Data";
            if (i<9) cout << "00";
            else if (i<99) cout << "0";
            cout << i+1 << ".txt:" << endl;
            SetConsoleTextAttribute(hConsole, 15);
            PreSnap(i);
            cntSnap++;
            if (cntSnap==5)         //Limit the number of snapshot
            {
                cout << endl << endl;
                SetConsoleTextAttribute(hConsole, 12);
                cout << "Enter \"s\" to skip to next query or \"c\" to continue on this query: ";
                cin >> c;
                SetConsoleTextAttribute(hConsole, 15);
                if (c=='s') break;
                else cntSnap=0;

            }
            cout << endl << endl;
        }
    }

    pSave.clear();
    pSaveIncomplete.clear();
    numDoc=0;
}


void Trie::Output(Node* pCur, char arr[100], int sl)
{
    for (int i=0; i<52; i++)
    {
        if (pCur->pNext[i]!=NULL)
        {
            sl++;
            arr[sl] = pCur->pNext[i]->value;
            Output(pCur->pNext[i],arr,sl);
            sl--;
        }
    }
    for (int i=1; i<=sl; i++) cout << arr[i];
    cout << endl;
}

Node* Trie::GetRoot()
{
    return pRoot;
}

void Trie::ProcessData()
{
    string s;
    stringstream ss;
    char word[20];
    for (int i=0; i<180; i++)
    {
        ss.str("");
        if (i<9) ss<<"00";
        else if (i<99) ss<<"0";
        ss<<i+1;
        s = "data/Data" + ss.str() + ".txt";
        strcpy(word,s.c_str());
        LoadData(word,i);
    }
}

void Trie::FreeTree(Node* pCur)
{
    for (int i=0; i<62; i++)
    {
        if (pCur->pNext[i]!=NULL)
            FreeTree(pCur->pNext[i]);
    }
    delete(pCur);
}

void Trie::Free()
{
    fclose(fres);
    FreeTree(pRoot);
}

void Trie::InitScore()
{
    for (int i=0; i<NDATA; i++)
    {
        ScoreRes[i].value=0;
        ScoreRes[i].docID=i;
    }

}

void Trie::RankOccurrence()      //Ranks document by occurrence
{
    for (int docID=0; docID<NDATA; docID++)
        if (OrRes[docID])
        {
            numDoc++;
            for (int i=0; i<pSave.size(); i++)
                ScoreRes[docID].value += pSave[i]->pos[docID].size()*hsOccurrence;
            ScoreRes[docID].value+=1;
        }
}

void Trie::RankConsecutiveAndNearest()  //Ranks document by consecutive and nearest
{
    bool stop=false, changed;
    int dmax, maxscore, score=0, maxval, sl=0;
    int Cur[NDATA],p[NDATA];
    for (int docID=0; docID<NDATA; docID++)
        if (OrRes[docID])
        {
            maxval=0; sl=0;
            maxscore=999999;
            stop=false;
            for (int i=0; i<pSave.size(); i++)
            {
                if (pSave[i]->pos[docID].size()>0)
                {
                    p[sl]=i;
                    sl++;
                }
            }
            dmax=0;
            for (int i=0; i<sl; i++)
            {
                Cur[i]=0;
                if (pSave[p[i]]->pos[docID][0] > pSave[p[dmax]]->pos[docID][0])    dmax=i;
            }
            while (!stop && sl!=0)
            {
                changed=false;
                score=1;
                maxval = pSave[p[dmax]]->pos[docID][Cur[dmax]];
                for (int i=0; i<sl; i++)
                    if (i!=dmax)
                    {
                        while ( (Cur[i]<pSave[p[i]]->pos[docID].size()-1)  &&
                                (abs(pSave[p[i]]->pos[docID][Cur[i]]-maxval) > abs(pSave[p[i]]->pos[docID][Cur[i]+1]-maxval)) )
                        /*do*/   Cur[i]++;
                    }
                for (int i=0; i<sl; i++)
                {
                    Node* pCur=pSave[p[i]];
                    if (i!=0)
                        score += abs(pSave[p[i]]->pos[docID][Cur[i]] - pSave[p[i-1]]->pos[docID][Cur[i-1]]);
                    if (pSave[p[i]]->pos[docID][Cur[i]] > pSave[p[dmax]]->pos[docID][Cur[dmax]])
                    {
                        changed=true;
                        dmax = i;
                    }
                }
                maxscore = min(score,maxscore);

                if (!changed)
                {
                    dmax=0;
                    for (int i=1; i<sl; i++)
                    {
                        if (Cur[i]<pSave[p[i]]->pos[docID].size()-1)
                        {
                            Cur[i]++;
                            changed=true;
                        }
                        if (pSave[p[i]]->pos[docID][Cur[i]] > pSave[p[dmax]]->pos[docID][Cur[dmax]]) dmax=i;
                    }
                }

                if (!changed) stop=true;
            }
            if(pSave.size()>0)
                ScoreRes[docID].value += (hsConsecutive/pSave.size()*sl + (hsConsecutive/100)/maxscore);
        }
}

bool ScoreCompare(Scoring a, Scoring b)
{
    return a.value>b.value;
}

void Trie::SortResult()     //Sort score in descending order
{
    sort(ScoreRes,ScoreRes+NDATA,ScoreCompare);
}

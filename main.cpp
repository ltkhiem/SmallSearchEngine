#include "header.h"

int main()
{
    bool stop=false;
    char cmd[1000], eoln;
    int hs[3];
    Trie Tree;
    do
    {
        SetConsoleTextAttribute(Tree.hConsole, 14);
        cout << "<Mini Search Engine> ";
        SetConsoleTextAttribute(Tree.hConsole, 15);
        scanf("%s",&cmd);
        if (strcmp(cmd, "set")==0)
        {
            while (scanf("%s%c",&cmd, &eoln)!=EOF)
            {
                if (strcmp(cmd,"+mc")==0)   Tree.SetMatchCase(true);
                if (strcmp(cmd,"-mc")==0)   Tree.SetMatchCase(false);
                if (strcmp(cmd,"+ss")==0)   Tree.SetSnapshot(true);
                if (strcmp(cmd,"-ss")==0)   Tree.SetSnapshot(false);
                if (strcmp(cmd,"+sw")==0)   Tree.SetStopWord(true);
                if (strcmp(cmd,"-sw")==0)   Tree.SetStopWord(false);
                if (eoln=='\n') break;
            }
        }
        if (strcmp(cmd, "rank")==0)
        {
            scanf("%d %d %d",&hs[0],&hs[1],&hs[2]);
            Tree.SetRank(hs);
        }
        if (strcmp(cmd, "run")==0)
        {
            Tree.InitTree();
            clock_t ProcessTime=clock();
            Tree.ProcessData();
            fprintf(Tree.fres,"%d ms\n",clock()-ProcessTime);
            Tree.LoadQuery();
            Tree.Free();
        }
        if (strcmp(cmd, "help")==0)
        {
            cout << endl;
            SetConsoleTextAttribute(Tree.hConsole, 10);
            cout << "Setting: " << endl;
            SetConsoleTextAttribute(Tree.hConsole, 15);
            cout << "set +mc" << setw(40) << "Enable matching word's case   " << endl;
            cout << "set -mc" << setw(40) << "Disable matching word's case  " << endl;
            cout << "set +ss" << setw(40) << "Enable displaying snapshot    " << endl;
            cout << "set -ss" << setw(40) << "Disable displaying snapshot   " << endl;
            cout << "set +mc" << setw(40) << "Enable eliminating stop words " << endl;
            cout << "set -mc" << setw(40) << "Disable eliminating stop words" << endl;
            cout << endl;
            SetConsoleTextAttribute(Tree.hConsole, 10);
            cout << "Ranking: " << endl;
            SetConsoleTextAttribute(Tree.hConsole, 15);
            cout << "rank x y z" << setw(65) << "Set the priority of ranking (x>y>z) with x,y,z from 1 to 3" << endl << endl;
            cout << setw(70) << "1: Ranks document that has words appear nearest to each others    " << endl;
            cout << setw(70) << "2: Ranks document that has words's occurrence highest             " << endl;
            cout << setw(70) << "3: Ranks document that has words appear in the same order in query" << endl;
            cout << endl;
            SetConsoleTextAttribute(Tree.hConsole, 10);
            cout << "Process: " << endl;
            SetConsoleTextAttribute(Tree.hConsole, 15);
            cout << "run" << setw(51) << "Run the engine to process the queries" << endl;
            cout << "exit" << setw(28) << "Exit the engine" << endl;
            cout << endl;
        }
        if (strcmp(cmd, "exit")==0)
            stop=true;
    }   while (!stop);

    return 0;
}

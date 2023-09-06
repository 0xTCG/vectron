#include <iostream>
#include <fstream>
#include <string>
//# pragma clang loop vectorize(assume_safety) vectorize_width(16)

int zdrop(int a, int b, int c){
    if(b - a <= c)
        return a;
    else
        return -32768;
}

int match_func(std::string x, int a, int b, int am, std::string y){
     if(x == "N" || y == "N"){
          return am;
     }
     else{
        if(x == y){
          return a;
        }
        else{
          return b;
        }
    }
}

int max_store(int **lst, int ind_row, int ind_col, int val_1, int val_2){
    lst[ind_row][ind_col] = std::max(val_1, val_2);
    return lst[ind_row][ind_col];
}

int max_val(int **a){
    int mx = 0;
    for(int i = 0; i < sizeof(a); i++){
        for(int j = 0; j < sizeof(a[i]); j++){
            if(mx < a[i][j]){
                mx = a[i][j];
            }
        }
    }
    return mx;  
}


int orig(std::string h, std::string m){
    int** q = 0;
    int** l = 0;
    int** s = 0;
    q = new int*[h.size() + 1];
    l = new int*[h.size() + 1];
    s = new int*[h.size() + 1];
    for (int k = 0; k < (h.size() + 1); k++){
        q[k] = new int[m.size() + 1];
        l[k] = new int[m.size() + 1];
        s[k] = new int[m.size() + 1];

        for(int p = 0; p < (m.size() + 1); p++){
            if(k == 0 && p == 0){
                q[k][p] = 0;
                l[k][p] = -10000;
                s[k][p] = -10000;             
            }
            else{
                if (k == 0 && p > 0){
                    q[k][p] = -4 + (-2 * p);
                    l[k][p] = -4 + (-2 * p);
                    s[k][p] = -10000;
                }
                else{
                    if(p == 0 && k > 0){
                        q[k][p] = -4 + (-2 * k);
                        l[k][p] = -10000;
                        s[k][p] = -4 + (-2 * k);
                    }
                    else{
                        q[k][p] = 0;
                        l[k][p] = -10000;
                        s[k][p] = -10000;
                    }
                }
            }
        }
    }
    std::string st1;
    std::string st2;
    for(int i = 1; i < (m.size() + 1); i++){        
        for(int j = 1; j < (h.size() + 1); j++){
            if(j - i <= -105 or j - i >= 105){
                if(j - i == -105 or j - i == 105){
                        q[i][j] = -10000;
                        l[i][j] = -10000;
                        s[i][j] = -10000;
                }
            }
            else{
                st1 = m[i - 1];
                st2 = h[j - 1];
                q[i][j] = std::max(std::max(q[i - 1][j - 1] + match_func(st1, 2, -4, -3, st2), max_store(s, i, j, s[i - 1][j] - 2, q[i - 1][j] - 6)), max_store(l, i, j, l[i][j - 1] - 2, q[i][j - 1] - 6));
                //q[i][j] = max(q[i - 1][j - 1] + match_func(m[i - 1], 5, 4, -3, h[j - 1]), l[i - 1][j - 1] + match_func(m[i - 1], 6, 4, -2, h[j - 1]), s[i - 1][j - 1] + match_func(m[i - 1], 7, 5, -3, h[j - 1]))                    
            }            
        }
    }
    std::cout << zdrop(q[512][512], max_val(q), 800) << "\n";
    return zdrop(q[512][512], max_val(q), 800);
}



int** prep(std::string *x, std::string *y){
    int** score = 0;
    score = new int*[64];
    for (int h = 0; h < 64; h++){
        score[h] = new int[64];
    }
    std::string a;
    std::string b;
    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 64; j++){
            a = x[i];
            b = y[j];
            score[i][j] = orig(a, b);
        }
    }
    return score;
}

int main(){
    int SEQ_NO_T = 64;
    int SEQ_NO_Q = 64;
    std::ifstream MyFile("seqx.txt");
    std::string seqs_x[SEQ_NO_T];
    int count = 0;
    for(int i = 0; i < SEQ_NO_T; i++){
        std::getline(MyFile, seqs_x[i]);
        seqs_x[i] += "\0";
        count++;
    }
    MyFile.close();
    MyFile.open("seqy.txt");
    std::string seqs_y[SEQ_NO_Q + 1];
    count = 0;
    for(int i = 0; i < SEQ_NO_Q; i++){
        std::getline(MyFile, seqs_y[i]);
        count++;
    }
    MyFile.close();
    int** d = 0;
    d = new int*[SEQ_NO_T];
    for (int h = 0; h < SEQ_NO_T; h++){
        d[h] = new int[SEQ_NO_Q];
        for(int l = 0; l < SEQ_NO_Q; l++){
            d[h][l] = 0;
        }
    }    
    std::cout << "BEGUN\n";    
    d = prep(seqs_x, seqs_y);
    std::cout << "DONE\n";
    return 0;
}

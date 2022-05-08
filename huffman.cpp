#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int ult_elem_heap = 0;

struct Noh_arvore{
    int peso;
    int fesq;
    int fdir;
    unsigned char c;
};

struct Noh_heap{
    int peso;
    int filho;
    unsigned char c;
    int pos_vetor;
};

int heap_fesq(int i){ return 2*i+1; }

int heap_fdir(int i){ return 2*i+2; }

int heap_pai(int i){ return (i-1)/2;}

template <typename T>
void heap_descer(T *vetor,int elem, int tamanho){    
    while (heap_fesq(elem) < tamanho && (vetor[elem].peso > vetor[heap_fesq(elem)].peso || vetor[elem].peso > vetor[heap_fdir(elem)].peso)){
        if (vetor[elem].peso > vetor[heap_fesq(elem)].peso || vetor[elem].peso > vetor[heap_fdir(elem)].peso){
            
            if(vetor[heap_fesq(elem)].peso < vetor[heap_fdir(elem)].peso) {
                T aux = vetor[elem];
                vetor[elem] = vetor[heap_fesq(elem)];
                vetor[heap_fesq(elem)] = aux;
                elem = heap_fesq(elem);
            }
            else{
                T aux = vetor[elem];
                vetor[elem] = vetor[heap_fdir(elem)];
                vetor[heap_fdir(elem)] = aux;
                elem = heap_fdir(elem);
            }
        }
    }
}

template <typename T>
void heap_subir(T *vetor, int elem){
    while(heap_pai(elem) >= 0 && vetor[elem].peso < vetor[heap_pai(elem)].peso){
        T aux = vetor[elem];
        vetor[elem] = vetor[heap_pai(elem)];
        vetor[heap_pai(elem)] = aux;
        elem = heap_pai(elem);
    }
}

template <typename T>
T heap_extrair_min(T *vetor, int tamanho){
   T max = vetor[0];
   vetor[0] = vetor[tamanho-1];
   heap_descer(vetor, 0, tamanho-1);
   return max; 
}

template <typename T>
void heap_criar(T *vetor, int tamanho){
   for(int i = tamanho-1; i >= 0; i--){
      heap_descer(vetor, i, tamanho);
   }
}

template <typename T>
void heap_inserir(T *vetor, T elem){
    vetor[ult_elem_heap] = elem;
    heap_subir(vetor, ult_elem_heap);
    ult_elem_heap++;
}

void visita_arvore(Noh_arvore *v, Noh_arvore raiz, string cod, string * v_cod){
    if(raiz.fesq == -1){
        v_cod[(int)raiz.c] = cod;
    }
    else{
        cod.push_back('0');
        visita_arvore(v, v[raiz.fesq], cod, v_cod);
        cod.pop_back();
        cod.push_back('1');
        visita_arvore(v, v[raiz.fdir], cod, v_cod);
    }
}

template <typename T>
bool gravar_vetor(T *v, int n, string str){
	ofstream arq(str,ofstream::app);
    if(!arq.is_open()) return false;

	char * fim = (char *) (v+n);

	for(char *p = (char *) v; p < fim; ++p){
		arq.put(*p);
	}
    return true;
}

int compactador(char * entrada, char * saida){
    //Abertura do arquivo a ser compactadoq
    ifstream file(entrada);
    if(!file.is_open()){
        cout << "Arquivo não existente" << endl;
        return 1;
    }
    
    //Criacao do vetor de ocorrencias
    int quant_char = 0;
    long int v[256];
    for(int i = 0; i < 256; i++){
        v[i] = 0;
    }
    int tam_arq = 0;
    while(1==1){
        unsigned char c = file.get();
        if(file.eof()) break;
        tam_arq++;
        if(v[c] == 0){
            quant_char++;
        }
        v[c]++;
    }

    //Caso 1: Arquivo vazio
    if(quant_char == 0){
        ofstream arq(saida);
        return 0;
    }
    //Caso 2: Arquivo com 1 byte
    else if(quant_char == 1){
        long n[1] = {1};
        int quant_c[1];
        char c[1];
        for(int i = 0; i < 256; i++){
            if(v[i] > 0){
                c[0] = i;
                quant_c[0] = v[i];
                break;
            }
        }
        bool teste;
        teste = gravar_vetor(n, 1, saida);
        if(teste == false){
            cout << "Erro ao compactar arquivo" << endl;
            return 1;
        }
        teste = gravar_vetor(quant_c, 1, saida);
        if(teste == false){
            cout << "Erro ao compactar arquivo" << endl;
            return 1;
        }
        teste = gravar_vetor(c, 1, saida);
        if(teste == false){
            cout << "Erro ao compactar arquivo" << endl;
            return 1;
        }
    }
    //Caso 3: Arquivo com 2 ou mais bytes
    else{
        //Declaracao da arvore de huffman e heap
        Noh_arvore * arvore = new(nothrow) Noh_arvore[(2*quant_char) - 1];
        Noh_heap * heap = new (nothrow) Noh_heap[quant_char];

        //criacao do heap de minimo
        heap_criar(heap, 0);
        for(int i = 0; i < 256; i++){
            if(v[i] > 0){
                Noh_heap noh;
                noh.peso = v[i];
                noh.c = i;
                noh.filho = 0;
                noh.pos_vetor = -1;
                heap_inserir(heap, noh);
            }
        }

        //Criacao da arvore de huffman
        int aux = 0;
        int quant_heap = quant_char;
        while(quant_heap > 1){
            Noh_heap x = heap_extrair_min(heap, ult_elem_heap);
            ult_elem_heap--;
            Noh_arvore nox;
            nox.peso = x.peso;
            nox.c = x.c;
            if(x.filho == 0){
                nox.fdir = -1;
                nox.fesq = -1;
                arvore[aux] = nox;
                x.pos_vetor = aux;  
                aux ++;
            }
            Noh_heap y = heap_extrair_min(heap, ult_elem_heap);
            ult_elem_heap--;
            Noh_arvore noy;
            noy.peso = y.peso;
            noy.c = y.c;
            if(y.filho == 0){
                noy.fdir = -1;
                noy.fesq = -1;
                arvore[aux] = noy;
                y.pos_vetor = aux;  
                aux ++;
            }

            Noh_heap z;
            Noh_arvore noz;
            z.peso = noz.peso = x.peso + y.peso;
            noz.fesq = y.pos_vetor;
            noz.fdir = x.pos_vetor;
            z.filho = 1;
            arvore[aux] = noz;
            z.pos_vetor = aux;  
            heap_inserir(heap, z);
            aux++;
            quant_heap--;
        }

        //CRIACAO DO VETOR DE CÓDIGOS DE CARACTERES (V_COD)
        string cod;
        string v_cod[256];

        visita_arvore(arvore, arvore[(2*quant_char) - 2], cod, v_cod);

        //ESCRITA DO PRELUDIO - GRAVACAO DA QUANTIDADE DE NOS DA ARVORE, TAMANHO DO ARQUIVO ORIGINAL E DA ARVORE NO ARQUIVO
        long int n[1] = {(2*quant_char) - 1};
        int tam[1] = {tam_arq};
        bool teste;
        teste = gravar_vetor(n, 1, saida);
        if(teste == false){
            cout << "Erro ao compactar arquivo" << endl;
            return 1;
        }
        teste = gravar_vetor(arvore, n[0], saida);
        if(teste == false){
            cout << "Erro ao compactar arquivo" << endl;
            return 1;
        }
        teste = gravar_vetor(tam, 1, saida);
        if(teste == false){
            cout << "Erro ao compactar arquivo" << endl;
            return 1;
        }
        
        //ESCRITA DO ARQUIVO COMPACTADO - DADOS
        ifstream file2 (entrada);
        ofstream arq_saida (saida, ofstream::app);
        
        int cont_bits = 0;
        int bytes_arq = 0;
        char byte_grav = 0;
        while(1==1){
            unsigned char a = file2.get();
            if(file2.eof()) break;
            string str = v_cod[(int)a];
            for(int i = 0; i < (int)str.size(); i++){
                if(cont_bits == 8){
                    cont_bits = 0;
                    arq_saida.put(byte_grav);
                    bytes_arq++;
                    byte_grav = 0;
                }
                unsigned char byte_aux = ((char)str[i] - 0x30);
                byte_grav = byte_grav << 1;
                byte_grav = byte_grav | byte_aux;
                cont_bits++;
            }
        }
        if(cont_bits > 0){
            byte_grav = byte_grav << (8 - cont_bits);
            arq_saida.put(byte_grav);
        }
    }
    return 0;
}

int descompactador(char * entrada, char * saida){
    long int n[1];
    ifstream arq (entrada);

    //LEITURA DA QUANTIDADE DE NOS DA ARVORE - USAR NO DESCOMPACTADOR
	char *fim = (char *) (n+1);

	for(char *p = (char *) n; p < fim; ++p){
		*p = arq.get();
        if(arq.eof()){
            ofstream file(saida);
            return 0;
        }
	}

    if(n[0] == 1){
        int quant_c[1];
        char *fim = (char *) (quant_c+1);

        for(char *p = (char *) quant_c; p < fim; ++p){
            *p = arq.get();
        }

        int c[1];
        char *fim2 = (char *) (c+1);

        for(char *p = (char *) c; p < fim2; ++p){
            *p = arq.get();
        }

        ofstream file(saida);
        for(int i = 0; i < quant_c[0]; i++){
            file.put(c[0]);
        }
        return 0;
    }

    //LEITURA DA ARVORE
    Noh_arvore * arv = new(nothrow) Noh_arvore[n[0]];
    if(arv == nullptr){
        cout << "Erro ao descompactar arquivo";
        return 1;
    }
    
    char *fim2 = (char *) (arv+n[0]);

	for(char *p = (char *) arv; p < fim2; ++p){
		*p = arq.get();
	}

    int arq_original[1];
    //LEITURA DA QUANTIDADE DE NOS DA ARVORE
	char *fim3 = (char *) (arq_original+1);

	for(char *p = (char *) arq_original; p < fim3; ++p){
		*p = arq.get();
	}

    ofstream file(saida);

    int noh_arv = n[0] - 1;
    int i = 0;
    while(i < arq_original[0]){
        unsigned char c = arq.get();
        if(arq.eof()) return 0;
        int j = 0;
        while (j < 8){
            if(arv[noh_arv].fesq == -1){
                file.put(arv[noh_arv].c);
                noh_arv = n[0] - 1;
                i++;
                if(i >= arq_original[0]) break;
            }
            else if(c < 128){
                noh_arv = arv[noh_arv].fesq;
                c = c << 1;
                j++;
            }
            else{
                noh_arv = arv[noh_arv].fdir;
                c = c << 1;
                j++;
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[]){
    if(argc != 4){
        cout << "Usage: ./programa [operacao] [entrada] [saida]" << endl;
        return 1;
    }   
    if(*(argv[1]) == 'c'){
        int a = compactador(argv[2], argv[3]);
        if(a != 0) cout << "Erro ao compactar" << endl;
    }
    if(*(argv[1]) == 'd'){
        int b = descompactador(argv[2], argv[3]);
        if(b != 0) cout << "Erro ao descompactar" << endl;
    }
    return 0;
}
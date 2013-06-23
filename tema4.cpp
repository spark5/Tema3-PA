/*Oncioiu Raluca
**Grupa: 323CA
**Tema3
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <list>
#include <deque>
#include <algorithm>

#define MAX 1000
#define MAX_VAL 255
#define NONE -1

using namespace std;

typedef struct { int index; int color; double cost; } pereche;
typedef struct { int index; int color; vector<pereche> copii;} node; 


//functie care calculeaza costul intre doi pixeli
double formula(int pixel, double medie, double abatere)
{
	double f, logaritm;
	
	logaritm = log(sqrt(2 * M_PI * pow(abatere, 2)));  
	f = 0.5 * pow((pixel - medie) / abatere, 2) + logaritm;
	
	if(f > 10)
	{
		return 10;
	}
		
	return f;
}

//algoritmul bfs pentru o matrice de pixeli
vector<int> bfs(vector<node> graf,
												 int source, 
												 int sink)
{
  //Ne vom folosi de vectorul de parinti pentru a spune daca un nod a fost
  //adaugat sau nu in coada
  vector<int> parent(graf.size() + 1, NONE);
  deque<int> q;
  q.push_back(source);
  
  while(parent[sink] == NONE && q.size() > 0) 
  {
    int node = q.front();
    q.pop_front();
		
		for(int i = 0; i < graf[node].copii.size(); i++)
    {
    	int copil = graf[node].copii[i].index;
    	
    	if(parent[copil] == NONE && graf[node].copii[i].cost > 0) 
      {
        parent[copil] = node;
        q.push_back(copil);
      }
    }
  }
  
  //daca nu s-a atins destinatia, atunci nu mai exista drumuri de crestere
  if(parent[sink] == NONE) 
	{
		return std::vector<int>();
  }

  //reconstituim drumul de la destinatie spre sursa
  vector<int> returnValue;
  
  for(int node = sink; true; node = parent[node]) 
  {
    returnValue.push_back(node);
    
    if(node == source) 
    {
      break;
    }
  }
	
  //inversam drumul pentru a incepe cu sursa si a se termina cu destinatia
  reverse(returnValue.begin(), returnValue.end());
 	
  return returnValue;
}

//functie care satureaza un drum
double saturate_path(vector<node> &graf, vector<int> &path)
{
  //determinam fluxul maxim prin drum
  double flow;
  
  for(int j = 0; j < graf[path[0]].copii.size(); j++)
  {
  	if(graf[path[0]].copii[j].index == path[1])
  	{
  		flow = graf[path[0]].copii[j].cost;
  		break;
  	}
  }
  
	for(int i = 1; i < path.size() - 1; ++i) 
  {
    int u = path[i];
    int v = path[i + 1];
   	
    for(int j = 0; j < graf[u].copii.size(); j++)
    {
    	if(graf[u].copii[j].index == v)
    	{
    		flow = (flow < graf[u].copii[j].cost ? flow : graf[u].copii[j].cost);
    		break;
    	}
    }
  }

  //si il saturam in graf
  for(int j = 0; j < path.size() - 1; ++j) 
  {
    int u = path[j];
    int v = path[j + 1];
    
    for(int i = 0; i < graf[u].copii.size(); i++)
    {
    	if(graf[u].copii[i].index == v)
    	{
    		graf[u].copii[i].cost -= flow;
    		break;
    	}
    }
 
    for(int i = 0; i < graf[v].copii.size(); i++)
    {
    	if(graf[v].copii[i].index == u)
    	{
    		graf[v].copii[i].cost += flow;
    		break;
    	}
    }
  }

  //Raportam fluxul cu care am saturat graful
  return flow;
}

//functie care calculeaza fluxul maxim printr-un graf
double maximum_flow(vector<node> &graf, int source, int sink)
{
  double returnValue = 0;
  double flow = 0, flow_sursa, flow_drena;
  int size = graf.size(), size_children, index;

	//se satureaza muchiile de forma sursa-nod-drena	
	for(int j = 0; j < graf[size - 2].copii.size(); j++)
	{
		flow_sursa = graf[size - 2].copii[j].cost;
		
		index = graf[size - 2].copii[j].index;
		size_children = graf[index].copii.size();
		flow_drena = graf[index].copii[size_children - 1].cost;
		
		if(flow_sursa < flow_drena)
		{
			flow = flow_sursa;
		}
		else
		{
			flow = flow_drena;
		}
				
		//se satureaza muchia sursa-nod
		graf[size - 2].copii[j].cost -= flow;
		
		//se satureaza muchia nod-drena
		graf[index].copii[size_children - 1].cost -= flow;
		
		returnValue += flow;
	}  
	
  //vom incerca in mod repetat sa determinam drumuri de crestere folosind
  //BFS si sa le saturam pana cand nu mai putem determina un astfel de drum in
  //graf
  while(true) 
  {
    //se determina drumul de crestere
   	vector<int> saturation_path = bfs(graf, source, sink);

    //daca nu exista drum de crestere, atunci intoarce fluxul maxim gasit
    if(saturation_path.size() == 0) 
    {
      break;
    }

    //altfel satureaza drumul
    returnValue += saturate_path(graf, saturation_path);
  }

  return returnValue;
}

//functie care gaseste taietura minima din graf si scrie in fisier pixelii 
void min_cut(vector<node> graf, int source, int n, int m)
{
  //facem o parcurgere BFS din nodul sursa si punem nodurile atinse de
  //parcurgere in source_set. Toate celelalte noduri se vor afla in
  //sink_set
  vector<bool> in_queue(graf.size(), false);
  deque<int> q;
  
  q.push_back(source);
  in_queue[source] = true;

  //rulam BFS din sursa si marcam nodurile atinse
 	while(q.size()) 
  {
    int node = q.front();
    q.pop_front();
    
  	for(int j = 0; j < graf[node].copii.size(); j++)
  	{
	    if(in_queue[j] == false && graf[node].copii[j].cost > 0) 
	    {
	      in_queue[j] = true;
	      q.push_back(graf[node].copii[j].index);
	    }
	  }
  }

	//se scriu in fisier pixelii considerati ca fac parte din plan sau fundal
	FILE *s = fopen("segment.pgm", "w");
	
	fprintf(s, "%s\n", "P2");
	fprintf(s, "%d %d\n", n, m);
	fprintf(s, "%d\n", MAX_VAL);

  //Traversam inca odata si impartim nodurile intre cele doua multimi
  for(int i = 1; i < graf.size() - 2; ++i) 
  {
    if(in_queue[i]) 
    {
			fprintf(s, "%d\n", 0);	
    } 
    else 
    {
      fprintf(s, "%d\n", 255);	
    }
  }
  
  fclose(s);
}

int main()
{	
	node sursa, drena;
	pereche copil;

	int m, n, lambda, max_val, treshold, px, px_fg, px_bg, **pixeli,
	 nr_fg = 0, nr_bg = 0;
	double sum_fg = 0, sum_bg = 0, medie_fg, medie_bg, abatere_fg, abatere_bg;
	char magic_nr[3];
	vector<int> v_fg, v_bg;

	FILE *f = fopen("parametri.txt", "r");
	FILE *fg = fopen("mask_fg.pgm", "r");
	FILE *bg = fopen("mask_bg.pgm", "r"); 
	FILE *img = fopen("imagine.pgm", "r");

	if(f == NULL || fg == NULL || bg == NULL || img == NULL)
	{
		perror("Can't open file");
		return 0;
	}

	//se citesc parametrii din fisier
	fscanf(f, "%d %d", &lambda, &treshold);
	fclose(f);
	
	//se citeste numarul magic si dimensiunile imaginilor
	fgets(magic_nr, 3, img);
	fscanf(img, "%d %d %d", &n, &m, &max_val);
	
	fgets(magic_nr, 3, fg);
	fscanf(fg, "%d %d %d", &n, &m, &max_val);
	
	fgets(magic_nr, 3, bg);
	fscanf(bg, "%d %d %d", &n, &m, &max_val);
	
	//se aloca memorie pentru matricea de pixeli
	pixeli = (int **) malloc( m * sizeof(int *));
	
	for(int i = 0; i < m; i++)
	{
		pixeli[i] = (int *) malloc(n * sizeof(int));
	}
	
	//se citesc pixelii din imagine & plan/fundal si se retin in lista de adiacenta
	for(int i = 0; i < m; i++)
	{
		for(int j = 0; j < n; j++)
		{
			fscanf(img, "%d", &px);
			fscanf(fg, "%d", &px_fg);
			fscanf(bg, "%d", &px_bg);
	
			//se calculeaza media pentru foreground	
			if(px_fg != 0)
			{
				sum_fg += px;
				v_fg.push_back(px);
				nr_fg++;
			}
			
			//se calculeaza media pentru background
			if(px_bg != 0)
			{
				sum_bg += px;
				v_bg.push_back(px);
				nr_bg++;
			}
			
			//se retine culoarea pixelului intr-o matrice
			pixeli[i][j] = px;
		}
	}
	
	medie_fg = sum_fg / nr_fg; 
	medie_bg = sum_bg / nr_bg;
	
	//se calculeaza abaterea foreground
	sum_fg = 0;
	
	for(int i = 0; i < nr_fg; i++)
	{
		sum_fg += pow(medie_fg - v_fg[i], 2); 
	}
	
	abatere_fg = sqrt(sum_fg / nr_fg);
	
	//se calculeaza abaterea background
	sum_bg = 0;
	
	for(int i = 0; i < nr_bg; i++)
	{
		sum_bg += pow(medie_bg - v_bg[i], 2); 
	}
	
	abatere_bg = sqrt(sum_bg / nr_bg);
	
	//nu se mai citeste nimic din imagini
	fclose(fg);
	fclose(bg);
	fclose(img);
	
	//se calculeaza graful pixelilor
	vector<node> graf(n * m + 3); 
	
	//se initializeaza nodurile sursa si drena
	sursa.index = m * n + 1;
	sursa.color = -1;
	
	drena.index = m * n + 2;
	drena.color = -1;
	
	for(int i = 0; i < m; i++)
	{
		for(int j = 0; j < n; j++)
		{
			node nod;
		
			nod.index = n * i + j + 1;
			nod.color = pixeli[i][j];
			
			//se calculeaza vecinii nodului curent si se adauga la lista lui de copii
			if(j + 1 < n && abs(pixeli[i][j] - pixeli[i][j + 1]) <= treshold)
			{
				copil.index = n * i + j + 2;
				copil.cost = lambda;
				copil.color = pixeli[i][j + 1];
				nod.copii.push_back(copil);
			}
			
			if(j - 1 > 0 && abs(pixeli[i][j] - pixeli[i][j - 1]) <= treshold)
			{
				copil.index = n * i + j;
				copil.cost = lambda;
				copil.color = pixeli[i][j - 1];
				nod.copii.push_back(copil);
			}
			
			if(i - 1 > 0 && abs(pixeli[i][j] - pixeli[i - 1][j]) <= treshold)
			{
				copil.index = n * (i - 1) + j + 1;
				copil.cost = lambda;
				copil.color = pixeli[i - 1][j];
				nod.copii.push_back(copil);
			}
			
			if(i + 1 < m && abs(pixeli[i][j] - pixeli[i + 1][j]) <= treshold)
			{
				copil.index = n * (i + 1) + j + 1;
				copil.cost = lambda;
				copil.color = pixeli[i + 1][j];
				nod.copii.push_back(copil);
			}
			
			//se adauga nodul curent la nodul sursa ca fiind nod copil
			copil.index = n * i + j + 1;
			copil.cost = formula(pixeli[i][j], medie_fg, abatere_fg);
			copil.color = pixeli[i][j];
			sursa.copii.push_back(copil);
			
			//se adauga nodul drena ca fiind nod copil pentru nodul curent
			copil.index = m * n + 2;
			copil.cost = formula(pixeli[i][j], medie_bg, abatere_bg);
			nod.copii.push_back(copil);
		
			//se adauga nodul in graf
			graf[nod.index] = nod;
		}
	}
	
	//se adauga nodurile sursa si drena
	graf[m * n + 1] = sursa;
	graf[m * n + 2] = drena;

	//edmonds-karp pentru flux maxim
	double flow = maximum_flow(graf, sursa.index, drena.index);

	//se calculeaza taietura minimala 
  min_cut(graf, sursa.index, n, m);

	return 0;
}


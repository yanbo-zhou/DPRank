//============================================================================
// Name        : movielens.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

/*
aps
*/
int O = 11;//the total number of papers in the dataset

int timeconv(string str);

struct nodeO
{
    int userId;
    int t;
	int dou;
    nodeO* next;
};

struct headO
{
	nodeO* cite;
	nodeO* cited;
	int t;
};

double disruption(headO* Olist, int n){
	int i,j,k,ni=0,nj=0,nk=0;
	int *sub=new int[O]; 
	int *subk=new int[O]; 
	for(i=0;i<O;i++){
		sub[i]=0;
		subk[i]=0;
	}
	nodeO *pcite,*pcited,*p;
	pcite = Olist[n].cite;
	pcited = Olist[n].cited;
	while(pcited){
		sub[pcited->userId]=1;
		pcited=pcited->next;
	}
	while(pcite){
		i=pcite->userId;
		p=Olist[i].cited;
		while(p){
			subk[p->userId]=1;
			p=p->next;
		}
		pcite=pcite->next;	
	}
	for(i=0;i<O;i++){
		if(sub[i]&&subk[i])
			nj++;
		else if(sub[i]&&!subk[i])
			ni++;
		else if(!sub[i]&&subk[i])
			nk++; 
	}
	delete []sub;
	delete []subk;

	if(ni+nj+nk>0)
		return (ni-nj)*1.0/(ni+nj+nk);
	else
		return 0;
 	  
}


int getresultDPrank(headO* Olist)
{

	int i,t;
	double* kout=new double[O];
	double* p=new double[O];
	double* vkpa=new double[O];
	double *disr=new double[O];
	int *age=new int[O];
	string e;
	ifstream rage1("id-date.txt");
	getline(rage1,e);

	while(rage1>>i){
		getline(rage1,e);
		age[i]=timeconv(e);
	}
	rage1.close();

    nodeO*q;
	
	ofstream res("result-DPrank",ios::app);

	for(i=0;i<O;i++){
		disr[i]=disruption(Olist,i);
	}	


	double alpha=0.94,ga=0.31;
	long double av,c,d;
//	for(alpha=0.7;alpha<1;alpha+=0.05)
//	for(ga=0;ga<=0.25;ga+=0.05)
	{
		for(i=0;i<O;i++)
		{ 
			vkpa[i]=1;
			kout[i]=0;
			p[i]=0;
		}
    	
		for(i=0;i<O;i++)
		{ 
		vkpa[i]=1;
		kout[i]=0;
		p[i]=0;
		}

		for(i=0;i<O;i++){
		q=Olist[i].cited;
		while(q){

			kout[q->userId]+=1;
			q=q->next;
		}
		}


		for(i=0;i<O;i++){
		vkpa[i]=1.0/O;
		}

	//DPrank iteration

		int j=0;
		while(j<50)
		{
		j++;
		av=0;
		for(i=0;i<O;i++){
			if(kout[i]==0){
				av+=alpha*vkpa[i];
			}
		}
		av=(av+1-alpha)/O;

		for(i=0;i<O;i++){
			d=pow(1-(disr[i]+1)/2.0,1);
			
			q=Olist[i].cited;
			while(q){
		
				if(kout[q->userId]>0)
				{

					t=(age[q->userId]-age[i]+1)>0?(age[q->userId]-age[i]+1):1;
					c=pow(t, -ga*d);
					p[i]+=alpha*vkpa[q->userId]*c/kout[q->userId];
				}
				q=q->next;
			}


			p[i]+=av;
		
		}

		for(i=0;i<O;i++){
			vkpa[i]=p[i];
			p[i]=0;
		}
		
		}
		res<<"paper id\t";
		res<<"DPrank score\n";
		for(i=0;i<O;i++){
			res<<i<<'\t'<<vkpa[i]<<endl;
		}
	
	}


	res.close();
	delete []kout;
	delete []p;
	delete []age;
	delete []vkpa;
	delete []disr;
	return 1;
}

/*
calculate the simirank result 2022.1.12
*/
/*时间字符串转化，转化为与1893-1相差的月数*/
int timeconv(string str)
{
	int y,m,d;
	sscanf(str.data(),"%d-%d-%d",&y,&m,&d);
//	cout<<y<<'\t'<<m<<'\t'<<d<<endl;
	return (y-1893)*12+(m-1);
}


headO * read(char * name)
{
	int i = 0, j,t,u,k;
    cout << 1 << endl;
    headO *Olist=new headO[O];
    cout << 2 << endl;

    for (i = 0; i < O; i++)
    {
		Olist[i].cite = NULL;
		Olist[i].cited = NULL;
    }

    ifstream rate(name);
    nodeO *q, *po,*p,*qa,*qb;
    int maxu = 0, maxo = 0;
    int tp=-1;
	string e;
	getline(rate,e);
    while (rate >> i >> j>>t)
    {
        if (i > maxu) maxu = i;
        if (j > maxo) maxo = j;
        q = new nodeO;
        q->userId = i;
        q->t = t;
		q->dou=1;
        q->next = NULL;

        if (Olist[j].cited && Olist[j].cited->t < q->t)
        {
            po = Olist[j].cited;
            while (po->next && po->next->t < q->t)
            {
                po = po->next;
            }
            q->next = po->next;
            po->next = q;
        }
        else
        {
            po = Olist[j].cited;
            q->next = po;
            Olist[j].cited = q;
        }
		p = new nodeO;
        p->userId = j;
        p->t = t;
		p->dou=1;
        p->next = NULL;

        if (Olist[i].cite && Olist[i].cite->userId < p->userId)
        {
            po = Olist[i].cite;
            while (po->next && po->next->userId < p->userId)
            {
                po = po->next;
            }
            p->next = po->next;
            po->next = p;
        }
        else
        {
            po = Olist[i].cite;
            p->next = po;
            Olist[i].cite = p;
        }
    }
	rate.close();
	return Olist;
}
void del(headO * Olist)
{
	int i;
	nodeO* q1,*q;
    for (i = 0; i < O; i++)
    {
        q = Olist[i].cited;
        while (q)
        {
            q1 = q;
            q = q->next;
            free(q1);
        }
    }
	 for (i = 0; i < O; i++)
    {
        q = Olist[i].cite;
        while (q)
        {
            q1 = q;
            q = q->next;
            free(q1);
        }
    }
    cout << 2 << endl;
	delete [] Olist;
}

void getresult()
{
    headO *Olist;
    Olist=read("citations-desc.txt");

	getresultDPrank(Olist);
    del(Olist);
}

int main()
{
	getresult();
	return 1;
}

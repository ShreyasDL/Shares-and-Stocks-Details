/****************************************************************************/
			  // PROJECT ON B-TREE//
/****************************************************************************/
/*** include headers ***/
#include<graphics.h>
#include <fstream.h>
#include <conio.h>
#include<iostream.h>
#include<iomanip.h>
#include <string.h>
#include <stdlib.h>
#include<dos.h>
#include<stdio.h>
#include<ctype.h>
#define indexfile "index.txt"
///////////////////////////////////////////////////////////////////////
fstream indfile;
int i,indsize;
int isAlpha(char ch)
{
	if(isalpha(ch) || ch==' ')
	    return 1;
	 else
	    return 0;
}

int checkShare_ID(char share_ID[])//check if share_ID is valid
{
	if((strlen(share_ID)==4)&&isdigit(share_ID[3])&&(isdigit(share_ID[2]))&&(isalpha(share_ID[0]))&&(isalpha(share_ID[1])))
		return 1;
	else
		return 0;
}

class block		// class for block
{
	  public:
		char keys[4][11];
		block *link;
		int disp[4];
		int cnt;
		block();
		~block(){}
		void split();
		void merge();
};
/*************************************************/
class SSET		// class for sequence set
{
	  public:
		typedef block *BK;
		SSET();
		~SSET(){}
		void create();
		int Insert(char*,int);
		void del(char*);
		block *FindNode(char*);
		void display();
};
/**************************************************/
class node	// class for btree node
{
	  public:
		char keys[4][11];
		node *dptrs[4];
		node *uptr;
		block *ptr[4];
		int cnt;
		node();
		~node(){}
		int isLeaf();
		void split(node *,node *,node *);
};
/**************************************************/
class btree     // class for btree
{
	  public:
		btree();
		int insert(char*,block *);
		node* findLeaf(char*,int&);
		block *search(char*,int&);
		void create();
		void dis(node*);
		~btree(){}
		node *root;
};
/************************************************/
class shares		// class for Blood_bank record
{
	  public:
		char share_ID[5],cust_name[20],comp_name[20],share_value[10],no_of_shares[20];
		/* char Blood_bank_location[15];
		 char Blood_bank_address[15];
		 char Blood_type_id[4];
		 char Donor_name[20];
		 char Blood_type[5];
		 char Donor_phone[15];
		 char Date[10];  */
		 //int share_value,no_of_shares;

		void Clear();
		int Unpack(fstream&);
		int Pack(fstream&);
		void Input();
		void Display();
		~shares(){}
		void Assign(shares&);
		void seqdisp();
};
//class IOBUF: clearing the buffer,
//parent class for varlen and delmfld buffer classes
class iobuf
{
	protected:
		char Buffer[100];
		int BufSiz;
	public:
		 void Clear();
		 virtual int Read(fstream&)=0;
		 virtual int Write(fstream&)=0;
		 virtual int Pack(shares&,fstream&)=0;
		 virtual int Unpack(shares&,fstream&)=0;
};
//varlen: defines read & write operation on files
class varlen : public iobuf
{
	  public:
		 int Read(fstream&);
		 int Write(fstream&);
};
//delmfld: defines pack & unpack operations for the buffer
class delmfld : public varlen
{
	public:
		 int Pack(shares&,fstream&);
		 int Unpack(shares&,fstream&);
};

class index
{
	public:
		char ishare_ID[15],addr[5];
		void initial();
		void write();
}in,id[50];

void index::initial()
{
	indfile.open(indexfile,ios::in);
	if(!indfile)
	{
		indsize=0;
		return;
	}
	for(indsize=0;;indsize++)
	{
		indfile.getline(id[indsize].ishare_ID,15,'|');
		indfile.getline(id[indsize].addr,5,'\n');
		if(indfile.eof())
			break;
	}
	indfile.close();
}
void index::write()
{
	indfile.open(indexfile,ios::out);
	for(i=0;i<indsize;i++)
		indfile<<id[i].ishare_ID<<"|"<<id[i].addr<<"\n";
	indfile.close();
}
int ind_search(char* code)
{
	int low=0,high=indsize-1;
	int mid;
	while(low<=high)
	{
		mid=(low+high)/2;
		if(strcmp(code,id[mid].ishare_ID)==0)
			return mid;
		else if(strcmp(code,id[mid].ishare_ID)>0)
			low=mid+1;
		else
			high=mid-1;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////
btree bt;
SSET s;
node *dummy,*dummy1;
block *head;
static int tot,usd;

//update record(s)
void update(char *key)
{
	shares stds[100],std;
	int f=0,found=0,g;
	char upd='n';
	fstream file("shares.txt",ios::in);
	file.seekg(0,ios::beg);
	while(!file.fail())
		if(std.Unpack(file))
			if(strcmpi(std.share_ID,key)==0)
			{
				found=1;
				cout<<"\n\tRecord:";
				std.Display();
				cout<<"\n\n Confirm permanent updation:[Y/N] ";
				cin>>upd;
				if(!(upd=='Y' || upd=='y'))
				{
					stds[f].Clear();
					stds[f++].Assign(std);
				}
				else
				{
					int pos=ind_search(std.share_ID);
					for(i=pos;i<indsize;i++)
						id[i]=id[i+1];
					indsize--;
					file.seekp(0,ios::end);
					pos=file.tellp();
					cout << "\n\t Enter the new record :\n";
					std.Input();
					for(i=indsize;i>0;i--)
					{
						if(strcmp(std.share_ID,id[i-1].ishare_ID)<0)
							id[i]=id[i-1];
						else
							break;
					}
					strcpy(id[i].ishare_ID,std.share_ID);
					itoa(pos,id[i].addr,10);
					indsize++;

					stds[f].Clear();
					stds[f++].Assign(std);
				}
			}
			else
			{
				stds[f].Clear();
				stds[f++].Assign(std);
			}
			file.clear();
			if(found==0)
				cout<<"\n\n\t Record not found...!\n";
			else
			{
				file.close();
				file.open("shares.txt",ios::out);
				file.seekp(0,ios::beg);
				for(g=0;g<f;g++)
					if(!(stds[g].Pack(file))) continue;
						file.clear();
			}
	file.close();
	delete head;
	delete bt.root;
	head = new block;
	bt.root = new node;
	s.create();
}
//search for given record with share_ID as key
void search(char *key)
{
	shares std;
	int found=0,i;
	block *dp;
	fstream file("shares.txt",ios::in);
	file.seekg(ios::beg);
	dp=bt.search(key,found);
	if(found==0)
		cout<<"\n\n\t Record not found...!\n";
	else
	{
		found=0;
		for(i=0;i<dp->cnt;i++)
			if(strcmpi(key,dp->keys[i])==0)
			{
				found = 1;
				file.seekg(dp->disp[i],ios::beg);
				std.Unpack(file);
				cout<<"\n\n\t Record found : ";
				std.Display();
			}
		if(found==0) cout<<"\n\n\t Record not found ";
	}
	file.clear();
	file.close();
}
void append()
{
	shares std;
	int flag=1, pos;
	fstream file("shares.txt",ios::app);
	std.Input();
	file.seekp(0,ios::end);
	pos=file.tellp();
	flag=s.Insert(std.share_ID,pos);
	if(flag && std.Pack(file)){
		for(i=indsize;i>0;i--)
		{
			if(strcmp(std.share_ID,id[i-1].ishare_ID)<0)
				id[i]=id[i-1];
			else
				break;
		}
		strcpy(id[i].ishare_ID,std.share_ID);
		itoa(pos,id[i].addr,10);
		indsize++;
		cout << "\n\t Done...\n";}
	else
		cout << "\n\t Failure.";
	file.clear();
	file.close();
}
//delete  record(s) from  file
void delrec(char *key)
{
	int r=0,found=0,s;
	char del='N';
	shares stds[100],std;
	fstream file("shares.txt",ios::in);
	file.seekg(0,ios::beg);

	while(!file.fail())
		if(std.Unpack(file))
			if(strcmpi(std.share_ID,key)==0)
			{
				found=1;
				cout<<" \n Record :";
				std.Display();
				cout<<"\n\n Confirm permanent deletion:[Y/N]";
				cin>>del;
				if(!(del=='Y' || del=='y'))
				{
					stds[r].Clear();
					stds[r++].Assign(std);
				}
				else
				{
					int pos=ind_search(std.share_ID);
					for(i=pos;i<indsize;i++)
						id[i]=id[i+1];
					indsize--;
					cout<<"\n\n\t Deleted : ";
				}
			}
			else
			{
				stds[r].Clear();
				stds[r++].Assign(std);
			}
			file.clear();
			if(found==0)
				cout<<"\n\n\t Record not found.";
			else
			{
				file.close();
				file.open("shares.txt",ios::out);
				file.seekp(0,ios::beg);
				for(s=0;s<r;s++)
				if(!(stds[s].Pack(file)))
					continue;
				file.clear();
			}
			file.close();
}

void driver(node *p)
{
	fstream fl;
	int i,j;
	char a[3]={'\0','\0','\0'};
	char st[1]={'\0'};
	char st1[1]={'\0'};
	char *tmp;
	shares std;
	strcpy(st,"|");
	strcpy(st1,"\n");
	if(p->isLeaf())
	{
		for(i=0;i<p->cnt;i++)
		{
			block *t=p->ptr[i];
			while(strlen(tmp))
				strcpy(tmp,"");
			for(j=0;j<t->cnt-1;j++)
			{
				itoa(t->disp[j],a,10);
				strcat(tmp,t->keys[j]);
				strcat(tmp,st);
				strcat(tmp,a);
				strcat(tmp,st);
			}
			itoa(t->disp[j],a,10);
			strcat(tmp,t->keys[j]);
			strcat(tmp,st);
			strcat(tmp,a);
			strcat(tmp,st1);
			strcat(tmp,p->keys[i]);
			strcat(tmp,'\0');
			fl.open("driver.txt",ios::app);
			fl.write(tmp,strlen(tmp));
			cout<<tmp<<"\n"<<strlen(tmp);
			fl.write("\n",1);
			fl.close();
		}
	}
	for(i=0;i<p->cnt;i++)
		if(p->dptrs[i]!=0)
			driver(p->dptrs[i]);
}

void display(node * p)
{
	int i,j;
	shares std;
	if(p->isLeaf())
	{
		fstream file("shares.txt",ios::in);
		if(file.fail())
		{
			gotoxy(24,10);
			cout<<"!!! ...The File Is Empty... !!!";
			getch();
			return;
		}
		cout<<"COUNT : "<<p->cnt;
		cout<<"\n********************************************************";
		for(i=0;i<p->cnt;i++)
		{
			block * t=p->ptr[i];
			for(j=0;j<t->cnt;j++)
			{
				file.seekg(t->disp[j],ios::beg);
				if(std.Unpack(file))
				{
					std.Display();
					cout<< "\n\t\t\t\t Press any key ...\n";
					cout<<"\n********************************************************";
					getch();
				}
				else
					break;
			}
		}
		file.clear();
		file.close();
	}
	for(i=0;i<p->cnt;i++)
		if(p->dptrs[i]!=0)
			display(p->dptrs[i]);
}
//////////////////////////////////////////////////////////////////////////
node::node()	// constructor for btree node
{
	for(int i=0;i<4;i++)
	{
		strcpy(keys[i],'\0');
		dptrs[i]=0;
		ptr[i]=0;
	}
	uptr=0;
	cnt=0;
}
/******************************************************/
int node::isLeaf()
{
	int flag=1;
	for(int i=0;i<4;i++)
		if((dptrs[i]) != 0)
		{
			flag=0;
			break;
		}
	return flag;
}
//nd : node to split;         cr : curr. level new node;
//up : upper level new node   rt : root of btree
//rt is for updating the root if it gets split
void node::split(node *nd,node *cr,node *up)
{
	int i;
	node *z=nd->uptr;//parent of nd
	cr=new node;
	for(i=2;i<4;i++)//move half of 'nd' to 'cr'
	{
		strcpy(cr->keys[cr->cnt],nd->keys[i]);
		strcpy(nd->keys[i],'\0');
		cr->ptr[cr->cnt]=nd->ptr[i];
		nd->ptr[i]=0;
		cr->dptrs[cr->cnt]=nd->dptrs[i];
		nd->ptr[i]=0;
		cr->cnt++;
		nd->cnt--;
	}
	if(z!=0)//nd is not root of b-tree
	{
		node *x2;
		if(z->cnt==4)//if upper level splits
		{
			dummy=0;
			dummy1=0;
			z->split(z,up,x2);//in upper level, up becomes cr. (recursive)
			if(dummy!=0) up=dummy;
			if(dummy1!=0) z=dummy1;
		///////////////////////////////
			for(i=2;i>=0;i--)//update z
			{
				if(strcmp((up->keys[i]),'\0')!=0)
					if(strcmp((nd->keys[(nd->cnt)-1]),(up->keys[i]))<0)
					{// shift the entries
						strcpy(up->keys[i+1],up->keys[i]);
						up->ptr[i+1]=up->ptr[i];
						up->dptrs[i+1]=up->dptrs[i];
					}
					else
					{//insert nd's highest value to z
						strcpy(up->keys[i+1],(nd->keys[(nd->cnt)-1]));
						up->ptr[i+1]=(nd->ptr[(nd->cnt)-1]);
						up->cnt++;
						break;
					}
			}
			if(i<0)
			{
				strcpy(up->keys[0],(nd->keys[(nd->cnt)-1]));
				up->ptr[0]=(nd->ptr[(nd->cnt)-1]);
				up->cnt++;
			}
			for(i=3;i>0 && strcmp((cr->keys[(cr->cnt)-1]),up->keys[i])!=0;i--);
				up->dptrs[i]=cr;
			cr->uptr=up;
	 ////////////////
		}
		else
		{
			for(i=2;i>=0;i--)//update z
			{
				if(strcmp((z->keys[i]),'\0')!=0)
					if(strcmp((nd->keys[(nd->cnt)-1]),(z->keys[i]))<0)
					{// shift the entries
						strcpy(z->keys[i+1],z->keys[i]);
						z->ptr[i+1]=z->ptr[i];
						z->dptrs[i+1]=z->dptrs[i];
					}
					else
					{//insert nd's highest value to z
						strcpy(z->keys[i+1],(nd->keys[(nd->cnt)-1]));
						z->ptr[i+1]=(nd->ptr[(nd->cnt)-1]);
						z->cnt++;
						break;
					}
			}
			if(i<0)
			{
				strcpy(z->keys[0],(nd->keys[(nd->cnt)-1]));
				z->ptr[0]=(nd->ptr[(nd->cnt)-1]);
				z->cnt++;
			}//z is updated
	 //change the ptr of z towards cr at required position
			for(i=3;i>0 && strcmp((cr->keys[(cr->cnt)-1]),z->keys[i])!=0;i--);
				z->dptrs[i]=cr;
			cr->uptr=nd->uptr;
		}
	}
	else//handle root
	{
		up=new node;
		strcpy(up->keys[0],nd->keys[(nd->cnt)-1]);
		up->ptr[0]=nd->ptr[(nd->cnt)-1];
		up->dptrs[0]=nd;
		strcpy(up->keys[1],cr->keys[(cr->cnt)-1]);
		up->ptr[1]=cr->ptr[(cr->cnt)-1];
		up->dptrs[1]=cr;
		up->cnt=2;
		nd->uptr=cr->uptr=up;
		bt.root=up;//reset root
	}
	dummy=cr;
	dummy1=nd;
}
//////////////////////////////////////////////////////////////////////////
btree::btree()		// construtor for btree
{
	root=new node;
}
/***************************************************/
//flg = 1 => already exists
node* btree::findLeaf(char* value,int &flg)
{
	node *x=root;
	int i;
	flg=0;
	while(!(x->isLeaf()))
	{
		for(i=3;i>=0;i--)
		{
			if((strcmp((x->keys[i]),'\0'))!=0)
				if(strcmp(value,(x->keys[i]))>0)
				{
					if(i<(x->cnt)-1)
						x=x->dptrs[i+1];
					else
						x=x->dptrs[(x->cnt)-1];
					break;
				}
				else if(strcmp(value,(x->keys[i]))==0)
					flg=1;
		}
		if(i<0) x=x->dptrs[0];
	}
	for(i=0;i<x->cnt;i++)
		if(strcmp(value,(x->keys[i]))==0)
		{
			flg=1;
			return x;
		}
	return x;
}
void btree::dis(node *p)
{
	int i;
	static int j=1;
	cout<< "\n  Level-" << j++ <<":";
	for(i=0;i<p->cnt;i++)
	cout << "    " << p->keys[i];
	cout <<"\n------------------------------------------------------------------------\n";
	for(i=0;i<p->cnt;i++)
		if(p->dptrs[i]!=0) dis(p->dptrs[i]);
	j--;
}

void btree::create()
{
	block *temp=head;
	bt.root = new node;
	while(temp!=0)
	{
		bt.insert(temp->keys[temp->cnt-1],temp);
		temp = temp->link;
	}
}
int btree::insert(char* val, block *d)
{
	int i,flg;
	node *x=findLeaf(val,flg),*a1,*a2;
	if(flg==1)
	{
    //cout << "\n\t Repeated! " << val << endl;
		return 0;
	}
	if((x->cnt)==4)//there is no space in the record so insert
	{
		x->split(x,a1,a2);//split the leaf and upper nodes if needed
		x=findLeaf(val,flg);
	}
	for(i=2;i>=0;i--)
		if(strcmp((x->keys[i]),'\0')!=0)
			if(strcmp(val,(x->keys[i]))<0)
			{
				strcpy(x->keys[i+1],x->keys[i]);
				x->ptr[i+1]=x->ptr[i];
	//forget about dptrs!
			}
			else
			{
				strcpy(x->keys[i+1],val);
				x->ptr[i+1]=d;
				x->cnt++;
				break;
			}
			if(i<0)
			{
				strcpy(x->keys[0],val);
				x->ptr[0]=d;
				x->cnt++;
			}
			else if(i==(x->cnt)-2)
				while((x->uptr)!=0)
				{
					x=x->uptr;
					strcpy(x->keys[(x->cnt)-1],val);
					x->ptr[(x->cnt)-1]=d;
				}
	return 1;
}
//displacement is retruned, flag 'found' is set
block *btree::search(char *key,int &fnd)
{
	int i,flg=0;
	fnd=0;
	node *x=findLeaf(key,flg),*a1,*a2;
	for(i=0;i<x->cnt;i++)
		if(strcmp(key,x->keys[i])<=0)
		{
			fnd=1;
			return(x->ptr[i]);
		}
	return head;
}
///////////////////////////////////////////////////////////////////////////
void iobuf::Clear()
{
	strcpy(Buffer,"");
	BufSiz=0;
}
////////////////////////////////////////////////////////////////////////////
//reads record into buffer & finds buffer length
int varlen::Read(fstream &file)
{
	if(file.fail()) return 0;
		Clear();
	file.getline(Buffer,100,'\n');
	BufSiz=strlen(Buffer);
	return 1;
}
//writes record into file & appends \n
int varlen::Write(fstream &file)
{
	if(file.write(Buffer,BufSiz))
	{
		file.write("\n",1);
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////
//pack the shares into Buffer with delimter and write into file
int delmfld::Pack(shares &std,fstream  &file)
{
	Clear();
	strcpy(Buffer,std.share_ID);
	strcat(Buffer,"|");
	strcat(Buffer,std.cust_name);
	strcat(Buffer,"|");
	strcat(Buffer,std.comp_name);
	strcat(Buffer,"|");
	strcat(Buffer,std.share_value);
	strcat(Buffer,"|");
	strcat(Buffer,std.no_of_shares);
	strcat(Buffer,"|");
	strcat(Buffer,'\0');
	BufSiz=strlen(Buffer);
	return(Write(file));
}
//read record from file into Buffer and unpack into shares
int delmfld::Unpack(shares &std,fstream &file)
{
	if(!Read(file)) return 0;
		std.Clear();
	if(BufSiz>0)
	{
		int p=0,q;
		for(q=0;Buffer[q]!='|';q++)
			std.share_ID[p++]=Buffer[q];
		std.share_ID[p]='\0';
		p=0;
		q++;
		for(;Buffer[q]!='|';q++)
			std.cust_name[p++]=Buffer[q];
		std.cust_name[p]='\0';
		p=0;
		q++;
		for(;Buffer[q]!='|';q++)
			std.comp_name[p++]=Buffer[q];
		std.comp_name[p]='\0';
		p=0;
		q++;
		for(;Buffer[q]!='|';q++)
			std.share_value[p++]=Buffer[q];
		std.share_value[p]='\0'; p=0; q++;
		for(;Buffer[q]!='|';q++)
			std.no_of_shares[p++]=Buffer[q];
		std.no_of_shares[p]='\0';
		p=0;
		q++;
	return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////
//clears shares object
void shares::Clear()
{
	strcpy(share_ID,"");
	strcpy(cust_name,"");
	strcpy(comp_name,"");
	strcpy(share_value,"");
	strcpy(no_of_shares,"");
}
//read data
void shares::Input()
{
	Clear();
	get_share_ID:
		 cout<<"\t Share ID      : ";
		 cin>>share_ID;
	if(checkShare_ID(share_ID)==0)
	{
		cout<<"\n INVALID share_ID\n";
		goto get_share_ID;
	}
	get_cust_name:
	cout<<"\n\t Customer Name     : ";
	gets(cust_name);
	if(cust_name[0] == '\0')
	{
		cout<<"Please Enter Customer Name : ";
		goto get_cust_name;
	}
	get_comp_name:
	cout<<"\n\t Company Name  : ";
	gets(comp_name);
	if(cust_name[0] == '\0')
	{
		cout<<"Please Enter Company Name : ";
		goto get_comp_name;
	}
	get_share_value:
	cout<<"\n\t Share_value : ";
	gets(share_value);
	if(cust_name[0] == '\0')
	{
		cout<<"Please Enter Share Value : ";
		goto get_share_value;
	}
	get_no_of_shares:
	cout<<"\n\t No_of_shares  : ";
	gets(no_of_shares);
	if(no_of_shares[0] == '\0')
	{
		cout<<"Please Enter No of shares : ";
		goto get_no_of_shares;
	}
}
//display 1 shares record
void shares::Display()
{
	cout << "\n\n\t Share ID    : " << share_ID
	   << "\n\n\tCustomer Name    : " << cust_name
	   << "\n\n\t Company Name   : " << comp_name
	   << "\n\n\t Share Value  : " << share_value
	   << "\n\n\t No_of_shares    : " << no_of_shares;
}
//assign given shares to this shares
void shares::Assign(shares &std)
{
	strcpy(share_ID,std.share_ID);
	strcpy(cust_name,std.cust_name);
	strcpy(comp_name,std.comp_name);
	strcpy(share_value,std.share_value);
	strcpy(no_of_shares,std.no_of_shares);
}
//packs shares into new buffer
int shares::Pack(fstream &file)
{
	delmfld buffer;
	return (buffer.Pack(*this,file));
}
//unpacks buffer into shares
int shares::Unpack(fstream &file)
{
	delmfld buffer;
	return (buffer.Unpack(*this,file));
}
///////////////////////////////////////////////////////////////////////////
block :: block()	// constructor for block
{
       for(int i=0;i<4;i++)
       {
		strcpy(keys[i],'\0');
		disp[i]=0;
       }
       cnt=0;
       link=0;
}
/***********************************************************************/
void block :: split()		// function to split the block
{
       block *newnode;
       newnode=new block;
       int j=0;
       for(int i=2;i<this->cnt;i++)
       {
		strcpy(newnode->keys[j],this->keys[i]);
		strcpy(this->keys[i],'\0');
		newnode->disp[j]=this->disp[i];
		this->disp[i]=-1;
		newnode->cnt++;
		j++;
       }
       newnode->link = this->link;
       this->link=newnode;
       this->cnt-=2;
       bt.create();
}
/***********************************************************************/
void block :: merge()		// function to merge two blocks
{
	block *t1,*t2;
	t1=head;
	if(t1 == this)        // merge first block
	{
		if(t1->link->cnt < 4 )  // check wether next block can be merged
		{
			for(int p=t1->link->cnt;p>=0;p--)
			{
				strcpy(t1->link->keys[p],t1->link->keys[p-1]);
				t1->link->disp[p] = t1->link->disp[p-1];
			}
			strcpy(t1->link->keys[0],t1->keys[0]);
			t1->link->disp[0]=t1->disp[0];
			t1->link->cnt++;
			head=t1->link;
			t1->link=0;
			delete t1;
		}
		else if(t1->link->cnt==4)     	// redistribution of keys
		{
			strcpy(t1->keys[1],t1->link->keys[0]);
			t1->disp[1]=t1->link->disp[0];
			t1->cnt++;
			for(int c=0;c<t1->link->cnt;c++)
			{
				strcpy(t1->link->keys[c],t1->link->keys[c+1]);
				t1->link->disp[c]=t1->link->disp[c+1];
			}
			t1->link->cnt--;
		}
	  }
	else		// find which block to be merged
	{
		while(t1->link != this)
			t1=t1->link;
		if(t1->cnt < 4)		// merge with left node
		{
			strcpy(t1->keys[t1->cnt],this->keys[0]);
			t1->disp[t1->cnt] = this->disp[0];
			t1->link = this->link;
			t1->cnt++;
			this->link=0;
			delete this;
		}
		else
		{
		if(this->link !=0)  	// check wether node to be merged is last
		{                      // if not
			t2=this->link;
			if(t2->cnt < 4)      // merge with right node
			{
				for(int i=t2->cnt;i>=0;i--)
				{
					strcpy(t2->keys[i],t2->keys[i-1]);
					t2->disp[i]=t2->disp[i-1];
				}
				strcpy(t2->keys[0],this->keys[0]);
				t2->disp[0]=this->disp[0];
				t2->cnt++;
				t1->link = this->link;
				this->link=0;
				delete this;
			}
			else
			{	//Redistribution
				t2=this->link;
				strcpy(this->keys[this->cnt],t2->keys[0]);
				this->disp[this->cnt]=t2->disp[0];
				this->cnt++;
				for(int i=0;i<t2->cnt;i++)
				{
					strcpy(t2->keys[i],t2->keys[i+1]);
					t2->disp[i]=t2->disp[i+1];
				}
				t2->cnt--;
			}
		}
		else			// if it is last block
		{
			strcpy(this->keys[1],this->keys[0]);
			this->disp[1]=this->disp[0];
			strcpy(this->keys[0],t1->keys[t1->cnt-1]);
			this->disp[0]=t1->disp[t1->cnt-1];
			this->cnt++;
			t1->cnt--;
		}
	}
}
}
/***********************************************************************/
void SSET::create()	// function to create blocks from file
{
	int pos;
	char *buf;
	fstream file;
	file.open("shares.txt",ios::in);
	if(file.fail())
	{
		clrscr();
		gotoxy(24,10);
		printf("!!! ...File is empty... !!!");
		gotoxy(10,15);
		printf("!!! ...Press any key to start entering the records... !!!");
		getch();
		return;
	}
	while(!file.fail())
	{
		pos=file.tellg();
		file.getline(buf,100,'|');
		if(strcmp(buf,"")!=0)
		{
			s.Insert(buf,pos);
			file.getline(buf,100,'\n');
		}
	}
	file.close();
	bt.create();
}
/*****************************************************************/
block *SSET :: FindNode(char *val)	// function to find the block
{
       BK t=head;
       int flag=0;
       node *temp=bt.root;
       int flg;
       temp = bt.findLeaf(val,flg);
       for(int i=0;i<temp->cnt;i++)
		if((strcmpi(temp->keys[i],val)>=0))
		{
			t=temp->ptr[i];
			flag=1;
			break;
		}
		else if((temp->cnt!=0)&& (flag!=1) &&(!(strcmpi(temp->keys[i],val)==0)))
			t=temp->ptr[temp->cnt-1];
       return t;
}
/************************************************************************/

int SSET :: Insert(char *val,int disp) // function to insert key in block
{
	int i=0;
	BK x=FindNode(val);
	for(i=0;i<x->cnt;i++)
	if(strcmpi(x->keys[i],val)==0)
	{
		cout<< " \nkey "<<val<<" is repeated \n"<<endl;
		return 0;
	}
	if(x->cnt < 4)
	{
		for(i=0;i<x->cnt;i++)
		if(strcmpi(x->keys[i],val)>0) break;
			if(strcmpi(x->keys[i],val)==0)
			{
				cout<< "\n key "<<val<<" is repeated \n"<<endl;
				return 0;
			}
			else
			{
				int d=i;
				i=x->cnt-1;
				while(i>=d)
				{
					strcpy(x->keys[i+1],x->keys[i]);
					x->disp[i+1]=x->disp[i];
					i--;
				}
				strcpy(x->keys[d],val);
				x->disp[d]=disp;
				x->cnt++;
				if(d==x->cnt-1)
				{
					bt.create();
				}
			}
		}
		else
		{
			x->split();
			x=FindNode(val);
			if(x->cnt < 4)
			{
				for(i=0;i<x->cnt;i++)
					if(strcmpi(x->keys[i],val)>=0) break;
						if(strcmpi(x->keys[i],val)==0)
	  //cout<< " key "<<val<<" is repeated "<<endl;
							cout<<"";
						else
						{
							int d=i;
							i=x->cnt-1;
							while(i>=d)
							{
								strcpy(x->keys[i+1],x->keys[i]);
								x->disp[i+1]=x->disp[i];
								i--;
							}
							strcpy(x->keys[d],val);
							x->disp[d]=disp;
							x->cnt++;
						}
			}
		bt.create();
       }
       return 1;
}
/**************************************************************************/
void SSET :: del(char *val)	// function to delete key from block
{
	int z,flg=0,i;
	BK x=FindNode(val);
	for(i=0;i<x->cnt;i++)
		if(strcmpi(x->keys[i],val)==0)
		{
			flg=1;
			z=i;
			break;
		}
		if(flg==1)
		{
			if(x->cnt-1 < 2)
			{
				for(int j=i;i<x->cnt;i++)
				{
					strcpy(x->keys[j],x->keys[i+1]);
					strcpy(x->keys[i+1],"");
					x->disp[j]=x->disp[i+1];
					x->disp[i+1]=-1;
					j++;
				}
				x->cnt--;
				x->merge();
				delete bt.root;
	 //bt.create();
			}
			else
			{
				for(int j=i;i<x->cnt;i++)
				{
					strcpy(x->keys[j],x->keys[i+1]);
					strcpy(x->keys[i+1],"");
					x->disp[j]=x->disp[i+1];
					x->disp[i+1]=-1;
					j++;
				}
				x->cnt--;
				if(z==x->cnt)
				{
					delete bt.root;
					bt.root = new node;
	  //bt.create();
				}
			}
		}
		else
			cout<<"\n\nKey "<<val<<" not found\n";
}
/**********************************************************************/
void SSET :: display()		// frunction to display nodes
{
	int j=0;
	BK t;
	t=head;
	getch();
	cout<<"***********************************************";
	cout<<"\n Block Structure \n";
	cout<<"***********************************************";
	while(t != 0)
	{
		cout<<"\n Node :"<<j;
		for(int i=0;i<t->cnt;i++)
		{
			cout<<"\n keys["<<i<<"] : " <<t->keys[i];
	 //   <<"\t disp["<<i<<"] : "<<t->disp[i]
//	 cout<<"\n";
		}
		t=t->link;
		j++;
	}
}
//////////////////////////////////////////////////////////////////////////
SSET :: SSET()		// constructor for sequence set
{
		 head = new block;
}
void shares::seqdisp()
{
	shares std;
	fstream file("shares.txt",ios::in);
	file.seekg(0,ios::beg);
	cout<<setiosflags(ios::left)<<setw(15)<<"Share ID"<<setw(15)<<"Customer"<<setw(15)<<"Company";
	cout<<setw(15)<<"Share Value"<<setw(15)<<"No_of_shares";
	cout<<"---------------------------------------------------------------------------"<<endl;
	while(!file.eof())
	{
		std.Unpack(file);
		cout<<setw(15)<<std.share_ID<<setw(15)<<std.cust_name<<setw(15)<<std.comp_name<<setw(15)<<std.share_value;
		cout<<setw(15)<<std.no_of_shares<<endl;

	}
	file.close();
}

/*************************************************/

int main ()
{
	textbackground(BLACK);
	textcolor(YELLOW);
	clrscr();
	int pos;
	char ch;
	shares stud;
	s.create();	//to do: check for driver
	char key[100];
	in.initial();
	clrscr();
	do
	{
		clrscr();
		gotoxy(18,2);
		cout<<"******************************************";
		gotoxy(18,4);
		cout<<"*";
		gotoxy(32,4);
		cout<<"MAIN MENU";gotoxy(59,4);cout<<"*";
		gotoxy(18,6);
		cout<<"******************************************";
		gotoxy(18,8);
		cout<<"* 1: Display all records using B-Tree";
		gotoxy(59,8);
		cout<<"*";
		gotoxy(18,10);
		cout<<"* 2: Add record into the file";
		gotoxy(59,10);
		cout<<"*";
		gotoxy(18,12);
		cout<<"* 3: Search for record using B-Tree";
		gotoxy(59,12);
		cout<<"*";gotoxy(18,14);
		cout<<"* 4: Delete record";
		gotoxy(59,14);
		cout<<"*";
		gotoxy(18,16);
		cout<<"* 5: Update record";
		gotoxy(59,16);
		cout<<"*";
		gotoxy(18,18);
		cout<<"* 6: B-Tree structure display";
		gotoxy(59,18);
		cout<<"*";
		gotoxy(18,20);
		cout<<"* 7: Entry Sequence display";
		gotoxy(59,20);
		cout<<"*";
		gotoxy(18,22);
		cout<<"* 8: Quit program";
		gotoxy(59,22);
		cout<<"*";
		gotoxy(18,24);
		cout<<"******************************************"<<endl;
		cout<<"\n\t\t Enter choice [1-8] : ";
		cin>>ch;
		switch(ch)
		{
			case '1': clrscr();
				  gotoxy(20,2);
				  cout<<"******************************************";
				  gotoxy(20,4);
				  cout<<"*  DISPLAY ALL RECORDS USING B-TREE  *";
				  gotoxy(20,6);
				  cout<<"******************************************\n\n";
				  display(bt.root);
				  break;
			case '2': clrscr();
				  gotoxy(22,2);
				  cout<<"******************************";
				  gotoxy(22,4);
				  cout<<"*  ADD RECORD INTO THE FILE  *";
				  gotoxy(22,6);
				  cout<<"******************************\n\n";
				  append();
				  in.write();
				  break;
			case '3': clrscr();
				  gotoxy(22,2);
				  cout<<"************************************";
				  gotoxy(22,4);
				  cout<<"*  SEARCH FOR RECORD USING B-TREE  *";
				  gotoxy(22,6);
				  cout<<"************************************\n\n";
				  cout<<"\n\t Enter the share_ID to search : ";
				  cin>>key;
				  search(key);
				  break;
			case '4': clrscr();
				  gotoxy(28,2);
				  cout<<"*******************";
				  gotoxy(28,4);
				  cout<<"*  DELETE RECORD  *";
				  gotoxy(28,6);
				  cout<<"*******************\n\n";
				  cout<<"\n\t Enter the Share_ID to delete : ";
				  cin>>key;
				  delrec(key);
				  s.del(key);
				  delete head;
				  head = new block;
				  bt.root = new node;
				  s.create();
				  in.write();
				  break;
			case '5': clrscr();
				  gotoxy(28,2);
				  cout<<"*******************";
				  gotoxy(28,4);
				  cout<<"*  UPDATE RECORD  *";
				  gotoxy(28,6);
				  cout<<"*******************\n\n";
				  cout<<"\n\t Enter the Share ID to update : ";
				  cin>>key;
				  update(key);
				  in.write();
				  break;
			case '6': clrscr();
				  gotoxy(20,2);
				  cout<<"**********************************";
				  gotoxy(20,3);
				  cout<<"*  B-TREE STRUCTURE DISPLAY  *";
				  gotoxy(20,4);
				  cout<<"**********************************\n\n";
				  bt.dis(bt.root);
				  s.display();
				  break;
			case '7': clrscr();
				  gotoxy(20,2);
				  cout<<"******************************************";
				  gotoxy(20,4);
				  cout<<"*  DISPLAY ALL RECORDS USING B-TREE  *";
				  gotoxy(20,6);
				  cout<<"******************************************\n\n";
				  stud.seqdisp();
				  break;
			case '8': cout << "\n\t\t !!!   Press any key to exit...   !!!";
				  break;
			default : cout<<"\n\t\t !!! Invalid Choice. Press any key ... !!!";
				  break;
		 }
		 if(ch>1 && ch<8)
		 cout << "\n\n\t Press any key to return to menu ... ";
		 getch();
	 }
	 while(ch!='8');
		return 0;
}



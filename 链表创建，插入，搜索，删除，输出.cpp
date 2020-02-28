#include <iostream>
using namespace std;
struct list
{
	int number;
	list *next;
};
list *creatlist()
{
	list *head=NULL,*q;
	int x;
	cout<<"please input a number,input -1 to quit"<<endl;
	cin>>x;
	while(x!=-1)
	{
		list *p=new list;
		p->number=x;
		p->next=NULL;
		if(head==NULL)
		{
			head=q=p;
		}
		else
		{
			q->next=p;
			q=p;
		}
		cin>>x;
	}
	return head;
}
void insert(list *&head,int a,int i)
{
	list *p=new list;
	if(i!=0&&i!=-1)
	{
		list *q=head;
		int j=1;
		while(j<i&&q->next!=NULL)
		{
			q=q->next;
			j++;
		}
		if(j==i)
		{
			p->number=a;
			p->next=q->next;
			q->next=p;
		}
		else
		{
			cout<<"Not found "<<i<<" node"<<endl;
		}
	}
	else
	{
		if(i==0)
		{
			p->number=a;
			p->next=head;
			head=p;
		}
		else
		{
			if(i==-1)
			{
				list *q=head;
				p->number=a;
				while(q->next!=NULL)
				{
					q=q->next;
				}
				q->next=p;
				p->next=NULL;
			}
			else
			{
				cout<<"Error"<<endl;
			}
		}
	}
}
void deletenode(list *head,int i)
{
	int j=1;
	list *q=head;
	if(i==1)
	{
		head=head->next;
		delete q;
	}
	else
	{
		while(j<i-1&&q->next!=NULL)
		{
			q=q->next;
			j++;
		}
		if(q->next!=NULL)
		{
			list *p=q->next;
			q->next=p->next;
			delete p;
		}
		else
		{
			cout<<"Not i node"<<endl;
		}
	}
}
void retrieve(list *head,int number)
{
	list *q=head;
	int count=0;
	while(q!=NULL)
	{
		count++;
		if(q->number==number)
		{
			break;
		}
		q=q->next;
	}
	if(q==NULL)
	{
		cout<<"Not found a node's value is "<<number<<endl;
	}
	else
	{
		cout<<count<<" node's value is "<<number<<endl;
	}
}
void input(list *head)
{
	list *q=head;
	cout<<q->number;
	q=q->next;
	while(q!=NULL)
	{
		cout<<" "<<q->number;
		q=q->next;
	}
}
void remove(list *head)
{
	while(head!=NULL)
	{
		list *q=head;
		head=head->next;
		delete q;
	}
}
int main()
{
	list *head=NULL;
	head=creatlist();
	cout<<"if you want to insert a number call 1"<<endl
	<<"if you want to delete a node call 2"<<endl<<"if you want to retrieve a value call 3"<<endl
	<<endl;
	int a;
	cin>>a;
	int number;
	switch(a)
	{
		case 1:
			int i;
			cout<<"  If input the number to head please input the number you want to insert and input 0 "<<endl
			<<"  If input the number to tail please input the number you want to insert and input -1"<<endl
			<<"  If input the number after a node please input the number you "<<endl<<"  want to insert and input the node you want to insert in the back"<<endl;
			cin>>number>>i;
			insert(head,number,i);
			input(head);
			remove(head);
			break;
		case 2:
			cout<<"please input the Node ordinal number you want to delete:"<<endl;
			cin>>number;
			deletenode(head,number);
			input(head);
			remove(head);
			break;
		case 3:
			cout<<"please input the number you want to retrieve:"<<endl;
			cin>>number;
			retrieve(head,number);
			break;
		default :
			cout<<"Error!"<<endl;
	}
	return 0;
	
}

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <iterator>

using namespace std;


int delete_nested_comment(string &strInput)
{
	int i = 0;
	string strTemp;
	int nIndex = strInput.length();
	stack<char> staReverse;
	stack<char> staForward;
	
	/* ����ջ��ɺ�ջ����ֻʣ��һ��б�� */
	while (nIndex > i)
	{
		switch (strInput[i])
		{
			case '/':
			{
				/* ѹ���һ��б�� */
				if ((staReverse.empty()) || ('*' != staReverse.top()))
				{
					staReverse.push(strInput[i]);
					i++;
				}
				/* ������б��ȫ��ɾ�� */
				else
				{
					i++;
				}
				
				break;
			}
			case '*':
			{
				staReverse.push(strInput[i]);
				i++;

				break;
			}

			default:
			{
				staReverse.push(strInput[i]);
				i++;

				break;
			}
		}
	}

	/* ��ջ��������ջʱȱ�ٵ�һ��б�� */
	if ('*' == staReverse.top())
	{
		staReverse.push('/');
		while (!staReverse.empty())
		{
			staForward.push(staReverse.top());
			staReverse.pop();
		}
	}
	else
	{
		while (!staReverse.empty())
		{
			if ('*' != staReverse.top())
			{
				staForward.push(staReverse.top());
				staReverse.pop();
			}

			if (('*' == staReverse.top()) && ('*' != staForward.top()))
			{
				static int nInsertFlg = 0;  //����ֻȱ��һ��б�ܣ�����ֻ����һ��
				if (0 == nInsertFlg)
				{
					staForward.push('/');
					nInsertFlg = 1;
				}
				
				staForward.push(staReverse.top());
				staReverse.pop();
			}
			//if ('*' == staReverse.top())
			else
			{
				staForward.push(staReverse.top());
				staReverse.pop();
			}
		}
	}

	while (!staForward.empty())
	{
		strTemp += staForward.top();
		staForward.pop();
	}

	strInput = strTemp;

	return 0;
}

int main(void)
{
	string strTemp;
	vector<string> vecInput;

	cout << "Please input your string, Q/q stop!" << endl;
	cin >> strTemp;
	while (("q" != strTemp))
	{
		vecInput.push_back(strTemp);
		cin >> strTemp;
	}
	
	vector<string>::iterator iter = vecInput.begin();
	cout << "Output result is:" << endl;
	for (iter = vecInput.begin(); iter != vecInput.end(); iter++)
	{
		delete_nested_comment(*iter);
		cout << (*iter) << endl;
	}
		
	
	return 0;
}
#include "stdafx.h"
#include<map>
#include<vector>
#include<string>
#include<stack>

using namespace std;

typedef struct 
{
	string	strStatusName;
	vector<char> oIn;
	vector<int>	 oEnd;
	int	nEnd;
	int	nBuild;
}STDFA;

typedef struct stNode
{
	char cWord;
	char cType;
	int	 nNum;
	struct stNode * pLf;
	struct stNode * pRt;
	vector<int> oFt;
	vector<int> oLs;
}STNODE;

typedef struct stFollow
{
	char cWord;
	int	nNum;
	map<int,int> oMapFollow;
}STFOLLOW;

static string gstrAlphabet="abcdefghijklmnopqrstuvwxyz";

class DFA
{
public:
	DFA(){m_pstTop=NULL;}
	void	createDAF(string pattern);
	int		matchPattern(const string& strWord);
private:
	void	buildTree(string pattern);
	void	addChar(char cChar);
	void	addFollowElement(const char cChar,int nNum);
private:
	STNODE*	createDotLeaf(int&);
	int		checkStatus();
	int		findStatus(string strStatus);
	string	createFirstStatus(int& nEnd);
private:
	void	addCatFollow(STNODE *p);
	void	addStarFollow(STNODE *p);
	void	createFollow();
	int		checkSpecialStar(STNODE *p);
private:
	STNODE* createStar(STNODE* pLf);
	STNODE* createLeaf(int i,char cWord);
	STNODE* createNode(STNODE* pLf,STNODE* pRt);
	void	crateDafEntry(string strStatus,int nEndStatus);
	int		createStatus(char cInput,string strStart,string& strStatus);
private:
	STNODE*	m_pstTop;
	map<char,char>		m_oMapChars;
	vector<STFOLLOW>	m_oFoll;
	vector<STDFA>		m_oDFA;
};

STNODE* DFA::createLeaf(int i,char cWord)
{
	STNODE*	p= new STNODE();
	p->cType=0;
	p->cWord = cWord;
	p->nNum = i;
	p->oFt.push_back(i);
	p->oLs.push_back(i);

	return p;
}

STNODE* DFA::createDotLeaf(int &nNum)
{
	STNODE*	p= new STNODE();
	p->cType=0;
	p->cWord = '.';
	p->nNum = 0;
	
	const char* pAl = gstrAlphabet.c_str();
	for(int i=0;i<gstrAlphabet.length();i++)
	{
		addChar(pAl[i]);
		addFollowElement(pAl[i],nNum);
		p->oFt.push_back(nNum);
		p->oLs.push_back(nNum);
		nNum++;
	}

	return p;
}
int DFA::checkSpecialStar(STNODE *p)
{
	for(int i =0;i<p->oFt.size();i++){
		int j = 0;
		for(;j<p->oLs.size();j++){
			if(p->oFt[i]==p->oLs[j])
				break;
		}

		if(j==p->oLs.size())
			return 0;
	}

	return 1;
}

STNODE*	DFA::createNode(STNODE* pLf,STNODE* pRt)
{
	STNODE*	p = new STNODE();

	p->cType=1;
	p->cWord = '&';
	p->nNum = 0;

	p->pLf = pLf;
	p->pRt = pRt;

	p->oFt = pLf->oFt;
	p->oLs = pRt->oLs;

	if(pLf->cWord=='*')
		for(int i = 0;i<pRt->oFt.size();i++)
			p->oFt.push_back(pRt->oFt[i]);
	
	if(pRt->cWord=='*')
		for(int i=0;i<pLf->oLs.size();i++)
			p->oLs.push_back(pLf->oLs[i]);

	
	if(pLf->cWord=='&'){
		if(pLf->oFt.size()==pLf->oLs.size()){
			int iRet = checkSpecialStar(pLf);
			if(iRet == 1){
				for(int i = 0;i<pRt->oFt.size();i++)
					p->oFt.push_back(pRt->oFt[i]);
			}	
		}
	}

	return p;
}

STNODE*	DFA::createStar(STNODE* pLf)
{
	STNODE*	p = new STNODE();

	p->cType=1;
	p->cWord = '*';
	p->nNum = 0;

	p->pLf = pLf;
	p->pRt = NULL;

	p->oFt = pLf->oFt;
	p->oLs = pLf->oLs;

	return p;
}

void	DFA::addChar(char cChar)
{
	if(cChar!='#')
		m_oMapChars[cChar]= cChar;
}

void	DFA::addFollowElement(const char cChar,int nNum)
{
	STFOLLOW stFollow;
	stFollow.cWord = cChar;
	stFollow.nNum = nNum;
	m_oFoll.push_back(stFollow);
}

void	DFA::buildTree(string pattern)
{
	STNODE*		p   = NULL;
	string		strSrc	= pattern+"#";
	const char* pSrc= strSrc.c_str();

	int		nLen = strSrc.length();
	int		i    =0;
	int		nNum =0;
	char	cChar;

	while(i < nLen)
	{
		STNODE* pLeaf = NULL;
		cChar = pSrc[i++];
		if(cChar!='.')
		{
			addChar(cChar);
			addFollowElement(cChar,nNum);
			pLeaf = createLeaf(nNum++,cChar);
		}
		else
			pLeaf = createDotLeaf(nNum);
		
		
		if(i < nLen){
			if(pSrc[i] == '*'){
				pLeaf = createStar(pLeaf);
				i++;
			}
		}

		if(m_pstTop == NULL){
			m_pstTop = pLeaf;
			continue;
		}

		STNODE* pCatNode = createNode(m_pstTop,pLeaf);
		m_pstTop = pCatNode;
	}
}

int		DFA::checkStatus()
{
	for(int i = 0;i< m_oDFA.size();i++){
		if(m_oDFA[i].nBuild == 0)
			return i;
	}

	return -1;
}

int		DFA::findStatus(string strStatus)
{
	for(int i = 0;i< m_oDFA.size();i++){
		if(m_oDFA[i].strStatusName.compare(strStatus) == 0)
			return i;
	}

	return -1;
}

void	DFA::addCatFollow(STNODE *p)
{
	STNODE*	pC1= p->pLf;
	STNODE* pC2= p->pRt;

	for(int i = 0;i< pC1->oLs.size();i++){
		for(int j =0;j<pC2->oFt.size();j++)
			m_oFoll[pC1->oLs[i]].oMapFollow[pC2->oFt[j]]=pC2->oFt[j];
	}
}

void	DFA::addStarFollow(STNODE *p)
{
	for(int i =0;i<p->oLs.size();i++){
		for(int j=0;j<p->oFt.size();j++)
			m_oFoll[p->oLs[i]].oMapFollow[p->oFt[j]] = p->oFt[j];
	}
		
}

void	DFA::createFollow()
{
	stack<STNODE*> oNodeStack;
	oNodeStack.push(m_pstTop);

	while(!oNodeStack.empty())
	{
		STNODE* p = oNodeStack.top();
		oNodeStack.pop();

		if(p == NULL)
			continue;

		if(p->cType == 0)
			continue;

		if(p->cWord !='*')
			addCatFollow(p);
		else
			addStarFollow(p);

		oNodeStack.push(p->pLf);
		oNodeStack.push(p->pRt);
	}
}

int		DFA::createStatus(char cInput,string strStart,string& strStatus)
{
	int				nTag =0;
	map<int,int>	oMapGather;
	const char*		pStart = strStart.c_str();
	
	for(int i = 0;i < m_oFoll.size();i++){
		if(m_oFoll[i].cWord == cInput){
			char szTmp[20]={0};
			sprintf_s(szTmp,",%d,",m_oFoll[i].nNum);
			int nPos = strStart.find(szTmp);
			if(nPos <0)
				continue;

			map<int,int>::iterator oIter = m_oFoll[i].oMapFollow.begin();
			for(;oIter!=m_oFoll[i].oMapFollow.end();oIter++)
				oMapGather[oIter->first] = oIter->first;
		}
	}
	map<int,int>::iterator oIter = oMapGather.begin();
	for(;oIter != oMapGather.end();oIter++){
		char szNum[20]={0};
		sprintf_s(szNum,"%d",oIter->first);
		strStatus += (string(szNum)+",");

		if(m_oFoll[oIter->first].cWord=='#')
		{
			nTag =1;
		}
	}
	if(strStatus.length()!=0)
		strStatus=","+strStatus;

	return nTag;
}

string	DFA::createFirstStatus(int& nEnd)
{
	string strStatus=",";
	for(int i =0;i<m_pstTop->oFt.size();i++){
		char szNum[20]={0};
		sprintf_s(szNum,"%d",m_pstTop->oFt[i]);
		strStatus+=(string(szNum)+",");

		if(m_oFoll[i].cWord=='#')
			nEnd = 1;
	}
	return strStatus;
}

void	DFA::crateDafEntry(string strStatus,int nEndStatus)
{
	STDFA stDaf;
	stDaf.nBuild =0;
	stDaf.nEnd =nEndStatus;
	stDaf.strStatusName= strStatus;
	m_oDFA.push_back(stDaf);
}

void	DFA::createDAF(string strPattern)
{
	buildTree(strPattern);
	createFollow();

	int nEnd  =0;
	string strStartStatus = createFirstStatus(nEnd);
	crateDafEntry(strStartStatus,nEnd);

	while(1)
	{
		int		i = checkStatus();
		if(i == -1)
			break;

		m_oDFA[i].nBuild =1;

		map<char,char>::iterator oIter = m_oMapChars.begin();
		for(;oIter != m_oMapChars.end();oIter++){
			string strStatus;

			int		nEndTag = createStatus(oIter->first,m_oDFA[i].strStatusName,strStatus);
			
			if(strStatus.length() == 0){
				m_oDFA[i].oIn.push_back(oIter->first);
				m_oDFA[i].oEnd.push_back(-1);
				continue;
			}

			int		iRet = findStatus(strStatus);
			if(iRet != -1){
				m_oDFA[i].oIn.push_back(oIter->first);
				m_oDFA[i].oEnd.push_back(iRet);
				continue;
			}

			crateDafEntry(strStatus,nEndTag);
			m_oDFA[i].oIn.push_back(oIter->first);
			m_oDFA[i].oEnd.push_back(m_oDFA.size()-1);
		}
	}

}

int		DFA::matchPattern(const string& strWords)
{
	const char* pSrc = strWords.c_str();
	int	nLen = strWords.length();
	int	nCurr =0;
	for(int i=0;i<nLen;i++){   
		int j = 0;
		for(;j<m_oDFA[nCurr].oIn.size();j++){
			if(m_oDFA[nCurr].oIn[j]==pSrc[i])
				break;
		}
		if(j == m_oDFA[nCurr].oIn.size())
			return -1;

		nCurr = m_oDFA[nCurr].oEnd[j];
		if(nCurr ==-1)
			return 0;
	}

	if( m_oDFA[nCurr].nEnd == 0)
		return 0;

	return 1;
}


int _tmain(int argc, _TCHAR* argv[])
{
	DFA test;
	
	test.createDAF(".c*.");
	int iRet = test.matchPattern("dddd");
	if(iRet == 1)
		printf("ok");
	else
		printf("false");

	getchar();
}
#pragma once

/**
* @file Navigation.h
* @author Nat Ryall
* @date 05/12/2008
*
* Copyright © SAPIAN
*/

//##############################################################################

// Global.
#include <Global.h>

//##############################################################################

// Shortcuts.
#define NavigationManager CNavigationManager::Get()

//##############################################################################

// Forward declare.
class CNavigationRequest;
class CNavigationMesh;
class CNavigationNode;
class CNavigationEvaluator;
class CNavigationManager;

// The possible pathfinding errors.
enum t_NavigationError
{
	NavigationError_Success,			// A path was successfully generated for the map.
	NavigationError_InvalidParam,		// An invalid parameter was passed to the function.
	NavigationError_LinkInvalid,		// The start or end link is invalid or are the same.
	NavigationError_LinkDisallowed,		// The start or end link are not allowed according to the evaluator.
	NavigationError_NoRoute,			// A route could not be found from the start node to the end node.
};

// The node status.
enum t_NavigationNodeStatus
{
	NavigationNodeStatus_None,
	NavigationNodeStatus_Open,
	NavigationNodeStatus_Closed,
};

//##############################################################################
class CNavigationRequest
{
	// Friends.
	friend class CNavigationManager;

public:
	// The navigation map to search.
	CNavigationMesh* m_pMesh;

	// The path starting node.
	CNavigationNode* m_pStart; 

	// The goal node.
	CNavigationNode* m_pGoal; 

	// The route evaluator to use.
	CNavigationEvaluator* m_pEvaluator;
};

//##############################################################################
class CNavigationMesh
{
	// Friends.
	friend class CNavigationManager;

public:
	// Constructor.
	CNavigationMesh(xint iNodes)
	{
		Allocate<CNavigationNode>(iNodes);
	}

	// Destructor.
	virtual ~CNavigationMesh();

	// Get the number of links this map has.
	inline xint GetNodeCount()
	{
		return (xint)m_lpNodes.size();
	}

	// Get a link by index.
	inline CNavigationNode* GetNode(xint iIndex)
	{
		XASSERT(iIndex < GetNodeCount());
		return m_lpNodes[iIndex];
	}

	// Get a link by index as the specified type.
	template<typename t_NodeType>
	inline t_NodeType* GetNodeAs(xint iIndex)
	{
		return (t_NodeType*)GetNode(iIndex);
	}

protected:
	// Types.
	typedef xarray<CNavigationNode*> t_NavigationNodeList;

	// Allocate a specified number of nodes for the map.
	template<typename t_NodeType>
	inline void Allocate(xint iCount)
	{
		XLISTERASEALL(m_lpNodes);

		m_lpNodes.clear();
		m_lpNodes.reserve(iCount);

		for (xint iA = 0; iA < iCount; ++iA)
		{
			t_NodeType* tNode = new t_NodeType();

			tNode->m_pMesh = this;
			tNode->m_iIndex = iA;

			m_lpNodes.push_back(tNode);
		}
	}

	// The list of all available links.
	t_NavigationNodeList m_lpNodes;
};

//##############################################################################
class CNavigationNode
{
	// Friends.
	friend class CNavigationMesh;
	friend class CNavigationManager;

public:
	// Destructor.
	virtual ~CNavigationNode() {}

	// Add a node link index.
	inline void LinkTo(xint iIndex)
	{
		XASSERT(iIndex < m_pMesh->GetNodeCount());
		m_lpLinks.push_back(iIndex);
	}

	// Add a node link.
	inline void LinkTo(CNavigationNode* pNode)
	{
		XASSERT(pNode->m_pMesh == m_pMesh && pNode != this);
		m_lpLinks.push_back(pNode->m_iIndex);
	}

	// Check if the specified node is linked to this node.
	xbool IsLinkedTo(CNavigationNode* pLink);

	// Get the number of connections this node has.
	inline xint GetLinkCount()
	{
		return (xint)m_lpLinks.size();
	}

	// Get a connected node by index.
	inline CNavigationNode* GetLink(xint iIndex)
	{
		XASSERT(iIndex < GetLinkCount());
		return m_pMesh->GetNode(m_lpLinks[iIndex]);
	}

	// Get the mesh this node belongs to.
	inline CNavigationMesh* GetMesh()
	{
		return m_pMesh;
	}

	// Get the local node index.
	inline xint GetIndex()
	{
		return m_iIndex;
	}

	// Get this node as the specified type.
	template<typename t_NodeType>
	inline t_NodeType* GetAs()
	{
		return (t_NodeType*)this;
	}

	// Set the data linked to the node.
	void SetData(void* pData) 
	{ 
		m_pData = pData;
	}

	// Get the data linked to the node.
	void* GetData() const 
	{ 
		return m_pData; 
	}

	// Get the data linked to the node as the specified type.
	template<typename t_DataType>
	t_DataType* GetDataAs() const 
	{ 
		return (t_DataType*)GetData(); 
	}

protected:
	// Types.
	typedef xarray<xint> t_IndexArray;

	// Constructor.
	CNavigationNode() {}

	// The mesh this node belongs to.
	CNavigationMesh* m_pMesh;

	// The node index in the mesh.
	xint m_iIndex;

	// Data linked to the node.
	void* m_pData;
	
	// The connected nodes.
	t_IndexArray m_lpLinks;
};

//##############################################################################
class CNavigationSearchNode
{
	// Friends.
	friend class CNavigationOpenList;
	friend class CNavigationManager;

protected:
	// Constructor.
	CNavigationSearchNode(CNavigationNode* pNode);

	// The referenced navigation node for this path node.
	CNavigationNode* m_pReference;

	// Specifies the node status.
	t_NavigationNodeStatus m_iStatus;

	// The parent path node.
	CNavigationSearchNode* m_pParent;

	// The last cost value for this node.
	xfloat m_fCost;

	// The last heuristic value for this node.
	xfloat m_fHeuristic;

	// The last fitness value for this node.
	xfloat m_fFitness;
};

//##############################################################################
class CNavigationOpenList
{
	// Friends.
	friend class CNavigationManager;

protected:
	// Types.
	typedef xlist<CNavigationSearchNode*> t_SearchNodeList;
	
	// Push a node onto the open list.
	void Push(CNavigationSearchNode* pNode);

	// Pop the next node from the open list.
	CNavigationSearchNode* Pop();

	// Remove the specified node from the open list.
	void Remove(CNavigationSearchNode* pNode);

	// Check if there are any nodes remaining in the node list.
	inline xbool IsEmpty()
	{
		return m_lpNodes.size() == 0;
	}

	// The internal node list.
	t_SearchNodeList m_lpNodes;
};

//##############################################################################
class CNavigationEvaluator
{
	// Friends.
	friend class CNavigationManager;

public:
	// Determine if the specified link is valid in this evaluator.
	virtual xbool IsAllowed(CNavigationRequest* pRequest, CNavigationNode* pNode) = 0;

	// Get the cost between the parent and current node.
	virtual xfloat GetCost(CNavigationRequest* pRequest, CNavigationNode* pParentNode, CNavigationNode* pCurrentNode) = 0;

	// Get the heuristic between the current and goal node.
	virtual xfloat GetHeuristic(CNavigationRequest* pRequest, CNavigationNode* pCurrentNode, CNavigationNode* pGoalNode) = 0;
};

//##############################################################################
class CNavigationPath
{
	// Friends.
	friend class CNavigationManager;

public:
	// Get the number of nodes this route has.
	inline xint GetNodeCount()
	{
		return (xint)m_lpNodes.size();
	}

	// Get a node by index.
	inline CNavigationNode* GetNode(xint iIndex)
	{
		XASSERT(iIndex < GetNodeCount());
		return m_lpNodes[iIndex];
	}

	// Get a node by index as the specified type.
	template<typename t_NodeType>
	inline t_NodeType* GetNodeAs(xint iIndex)
	{
		return (t_NodeType*)GetNode(iIndex);
	}

	// Get the current node.
	inline CNavigationNode* GetCurrentNode()
	{
		return *m_pIterator;
	}

	// Get the current node as the specified type.
	template<typename t_NodeType>
	inline t_NodeType* GetCurrentNodeAs(xint iIndex)
	{
		return (t_NodeType*)GetCurrentNode(iIndex);
	}

	// Get the next node in the sequence.
	inline CNavigationNode* GetNextNode()
	{
		if (m_pIterator != m_lpNodes.end())
			return (++m_pIterator == m_lpNodes.end()) ? NULL : *m_pIterator;

		return NULL;
	}

	// Get the next node in the sequence as the specified type.
	template<typename t_NodeType>
	inline t_NodeType* GetNextNodeAs(xint iIndex)
	{
		return (t_NodeType*)GetNextNode(iIndex);
	}

protected:
	// Types.
	typedef xarray<CNavigationNode*> t_NavigationNodeList;

	// The current node (for convenient tracking).
	t_NavigationNodeList::iterator m_pIterator;

	// The route nodes.
	t_NavigationNodeList m_lpNodes;
};

//##############################################################################
class CNavigationManager : public CModule
{
public:
	// Singleton instance.
	static inline CNavigationManager& Get() 
	{
		static CNavigationManager s_Instance;
		return s_Instance;
	}

	// Find a path from the source node to the destination node.
	t_NavigationError FindPath(CNavigationRequest* pRequest, XOUT CNavigationPath& xPath);

protected:
	// Types.
	typedef xarray<CNavigationSearchNode*> t_SearchNodeArray;

	// Create a new search node list.
	void CreateSearchNodes(CNavigationMesh* pMesh, XOUT t_SearchNodeArray& lNodes);

	// Free the specified search node list.
	void FreeSearchNodes(t_SearchNodeArray& lNodes);
};
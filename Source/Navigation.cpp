//##############################################################################

// Global.
#include <Global.h>

// Local.
#include <Navigation.h>

//##############################################################################

// =============================================================================
CNavigationMesh::~CNavigationMesh()
{
	XLISTERASEALL(m_lpNodes);
}

//##############################################################################

// =============================================================================
xbool CNavigationNode::IsLinkedTo(CNavigationNode* pLink)
{
	XLISTFOREACH(t_IndexArray, pIndex, m_lpLinks)
	{
		if (GetLink(*pIndex) == pLink)
			return true;
	}

	return false;
}

//##############################################################################

// =============================================================================
CNavigationSearchNode::CNavigationSearchNode(CNavigationNode* pNode) : 
	m_pReference(pNode),
	m_iStatus(NavigationNodeStatus_None),
	m_pParent(NULL),
	m_fCost(0.f),
	m_fHeuristic(0.f),
	m_fFitness(0.f)
{
}

//##############################################################################

// =============================================================================
void CNavigationOpenList::Push(CNavigationSearchNode* pNode)
{
	// Add to the open list, lowest cost first.
	for (t_SearchNodeList::iterator ppNode = m_lpNodes.begin(); true; ppNode++)
	{
		if (ppNode == m_lpNodes.end() || pNode->m_fFitness < (*ppNode)->m_fFitness)
		{
			m_lpNodes.insert(ppNode, pNode);
			return;
		}
	}
}

// =============================================================================
CNavigationSearchNode* CNavigationOpenList::Pop()
{
	CNavigationSearchNode* pNode = m_lpNodes.front();
	m_lpNodes.pop_front();

	return pNode;
}

// =============================================================================
void CNavigationOpenList::Remove(CNavigationSearchNode* pNode)
{
	m_lpNodes.remove(pNode);
}

//##############################################################################

// =============================================================================
t_NavigationError CNavigationManager::FindPath(CNavigationRequest* pRequest, XOUT CNavigationPath& xPath)
{
	// Check the info is valid.
	if (!pRequest)
		return NavigationError_InvalidParam;

	if (!pRequest->m_pStart || !pRequest->m_pGoal || pRequest->m_pStart == pRequest->m_pGoal)
		return NavigationError_LinkInvalid;

	if (!pRequest->m_pEvaluator->IsAllowed(pRequest, pRequest->m_pStart) || !pRequest->m_pEvaluator->IsAllowed(pRequest, pRequest->m_pGoal))
		return NavigationError_LinkDisallowed;

	// Create the search node array.
	t_SearchNodeArray lpSearchNodes;
	CreateSearchNodes(pRequest->m_pMesh, lpSearchNodes);

	// Create the start node for the path and intitialise.
	CNavigationSearchNode* pStartNode = lpSearchNodes[pRequest->m_pStart->m_iIndex];

	// Create the node lists and add the first node.
	CNavigationOpenList xOpenList;
	xOpenList.Push(pStartNode);

	// Search for a path to the goal node.
	CNavigationSearchNode* pWorkingNode = NULL;

	while (!xOpenList.IsEmpty())
	{
		// Get the lowest fitness node and close it.
		pWorkingNode = xOpenList.Pop();
		pWorkingNode->m_iStatus = NavigationNodeStatus_Closed;

		// Check if we have reached our goal.
		if (pWorkingNode->m_pReference == pRequest->m_pGoal)
			break;

		// Add all valid nodes connected to the working node to the open list.
		for (xint iA = 0; iA < pWorkingNode->m_pReference->GetLinkCount(); ++iA)
		{
			CNavigationNode* pLinkedNode = pWorkingNode->m_pReference->GetLink(iA);
			CNavigationSearchNode* pSearchNode = lpSearchNodes[pLinkedNode->m_iIndex];
		
			if (pSearchNode->m_iStatus != NavigationNodeStatus_Closed)
			{
				// Check that the node is allowed.
				if (!pRequest->m_pEvaluator->IsAllowed(pRequest, pLinkedNode))
				{
					pSearchNode->m_iStatus = NavigationNodeStatus_Closed;
					continue;
				}

				// If the node is already open, we should determine if the current path is cheaper.
				if (pSearchNode->m_iStatus == NavigationNodeStatus_Open)
				{
					float fCost = pRequest->m_pEvaluator->GetCost(pRequest, pWorkingNode->m_pReference, pSearchNode->m_pReference) + pWorkingNode->m_fCost;

					// If this path to the node is cheaper, use it instead.
					if (fCost < pSearchNode->m_fCost)
					{
						pSearchNode->m_pParent = pWorkingNode;
						pSearchNode->m_fCost = fCost;
						pSearchNode->m_fFitness = pSearchNode->m_fCost + pSearchNode->m_fHeuristic;

						// Remove the node from the open list and re-add it.
						xOpenList.Remove(pSearchNode);
						xOpenList.Push(pSearchNode);
					}
				}
				// Otherwise just add the node.
				else
				{
					// Update the node with the calculated fitness.
					pSearchNode->m_iStatus = NavigationNodeStatus_Open;
					pSearchNode->m_pParent = pWorkingNode;
					pSearchNode->m_fCost = pRequest->m_pEvaluator->GetCost(pRequest, pWorkingNode->m_pReference, pSearchNode->m_pReference) + pWorkingNode->m_fCost;
					pSearchNode->m_fHeuristic = pRequest->m_pEvaluator->GetHeuristic(pRequest, pSearchNode->m_pReference, pRequest->m_pGoal);
					pSearchNode->m_fFitness = pSearchNode->m_fCost + pSearchNode->m_fHeuristic;

					// Add the node to the open list.
					xOpenList.Push(pSearchNode);
				}
			}
		}
	}

	// Copy the path to the output list.
	xPath.m_lpNodes.clear();

	while (pWorkingNode)
	{
		xPath.m_lpNodes.push_back(pWorkingNode->m_pReference);
		pWorkingNode = pWorkingNode->m_pParent;
	}

	reverse(xPath.m_lpNodes.begin(), xPath.m_lpNodes.end());
	xPath.m_pIterator = xPath.m_lpNodes.size() ? xPath.m_lpNodes.begin() : xPath.m_lpNodes.end();

	// Clean up the memory footprint.
	FreeSearchNodes(lpSearchNodes);
		
	// Return the result.
	return xPath.m_lpNodes.size() ? NavigationError_Success : NavigationError_NoRoute;
}

// =============================================================================
void CNavigationManager::CreateSearchNodes(CNavigationMesh* pMesh, t_SearchNodeArray& lpNodes)
{
	lpNodes.clear();
	lpNodes.reserve(pMesh->GetNodeCount());

	for (xint iA = 0; iA < pMesh->GetNodeCount(); ++iA)
	{
		lpNodes.push_back(new CNavigationSearchNode(pMesh->GetNode(iA)));
	}
}

// =============================================================================
void CNavigationManager::FreeSearchNodes(t_SearchNodeArray& lpNodes)
{
	XLISTERASEALL(lpNodes);
}

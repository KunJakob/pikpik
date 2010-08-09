#pragma once

/**
* @file Transition.h
* @author Nat Ryall
* @date 09/08/2010
* @brief Manages full screen transitions.
*
* Copyright © Sketch Spark
*/

//##############################################################################

// Global.
#include <Global.h>

//##############################################################################
class CTransitionScreen : public CScreen
{
public:
    // Constructor.
    CTransitionScreen(t_ScreenIndex iNextScreen);

    // Destructor.
    virtual ~CTransitionScreen();
};

//##############################################################################
//#########################################################################
// 
//	*******************************************************************
//	File: Event Template
//	*******************************************************************
//	Part of the Xen Engine.
// 
//	Author: Nat Ryall
//	Creation Date: Sunday, 5 March 2006 (12:26 AM [GMT +0])
//	Refactor Date: Thursday, 8 February 2007 (09:32 PM [GMT +10])
//	-------------------------------------------------------------------
//	Copyright © 1998 - 2007, SAPIAN
// 
//#########################################################################

#ifndef __XEN__TemplatesEvent_T_h__
#define __XEN__TemplatesEvent_T_h__

//#########################################################################

///////////////////////////////////////////////////////////////////////////
// 
//	EVENT BASE
// 
///////////////////////////////////////////////////////////////////////////
namespace Xen
{
	namespace Templates
	{
		template <typename t_Return, typename t_Arguments>
		class CEventBaseT 
		{
		public:
			// Destructor.
			virtual	~CEventBaseT() {}

			// The event callback prototype.
			virtual t_Return Callback(t_Arguments in_Arguments) = 0;
		};
	}
}

///////////////////////////////////////////////////////////////////////////
// 
//	MEMBER EVENT
// 
///////////////////////////////////////////////////////////////////////////
namespace Xen
{
	namespace Templates
	{
		template <typename t_Class, typename t_Return, typename t_Arguments>
		class CMemberEventT : public CEventBaseT<t_Return, t_Arguments> 
		{
		public:
			// The class function pointer.
			typedef t_Return (t_Class::*t_pFunction)(t_Arguments);

			// Constructor.
			CMemberEventT(t_Class* in_Instance, t_pFunction in_Function) :
			m_pClass(in_Instance),
				m_pFunction(in_Function)
			{}

			// The event callback.
			virtual t_Return Callback(t_Arguments in_Arguments) 
			{
				return (m_pClass->*m_pFunction)(in_Arguments);
			}

		private:
			// The owner class.
			t_Class* m_pClass;

			// The callback function.
			t_pFunction m_pFunction;
		};
	}
}

///////////////////////////////////////////////////////////////////////////
// 
//	EVENT
// 
///////////////////////////////////////////////////////////////////////////
namespace Xen
{
	namespace Templates
	{
		template <typename t_Return, typename t_Arguments>
		class CEventT 
		{
		public:
			// Constructor: Member function.
			template <typename t_Class>
			CEventT(t_Class* in_Instance, t_Return (t_Class::*in_Function)(t_Arguments)) {
				m_pEvent = new CMemberEventT<t_Class, t_Return, t_Arguments>(in_Instance, in_Function);
			}

			// Destructor.
			~CEventT() {
				delete m_pEvent;
			}

			// Overload "()", calls the event function.
			inline t_Return	operator() (t_Arguments in_Arguments) 
			{
				return m_pEvent->Callback(in_Arguments);
			}

		private:
			// The event to be called.
			CEventBaseT<t_Return, t_Arguments>* m_pEvent;
		};
	}
}

//#########################################################################

#endif // __XEN__TemplatesEvent_T_h__

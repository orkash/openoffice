/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_dbaccess.hxx"
#ifndef DBAUI_TABLEWINDOWTITLE_HXX
#include "TableWindowTitle.hxx"
#endif
#ifndef DBAUI_TABLEWINDOW_HXX
#include "TableWindow.hxx"
#endif
#ifndef DBAUI_QUERYTABLEVIEW_HXX
#include "QueryTableView.hxx"
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SV_HELP_HXX
#include <vcl/help.hxx>
#endif
#ifndef _SV_MENU_HXX
#include <vcl/menu.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef DBACCESS_SHARED_DBUSTRINGS_HRC
#include "dbustrings.hrc"
#endif
#ifndef _CNTIDS_HRC
#include <sfx2/cntids.hrc>
#endif
#ifndef DBAUI_TABLEWINDOWLISTBOX_HXX
#include "TableWindowListBox.hxx"
#endif
#ifndef DBAUI_TABLECONNECTION_HXX
#include "TableConnection.hxx"
#endif
#ifndef _DBU_QRY_HRC_
#include "dbu_qry.hrc"
#endif
#ifndef DBAUI_QUERYDESIGNVIEW_HXX
#include "QueryDesignView.hxx"
#endif
#ifndef DBAUI_JOINCONTROLLER_HXX
#include "JoinController.hxx"
#endif

#include <algorithm>

using namespace dbaui;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::uno;
//==================================================================
// class OTableWindowTitle
//==================================================================
DBG_NAME(OTableWindowTitle)
//------------------------------------------------------------------------------
OTableWindowTitle::OTableWindowTitle( OTableWindow* pParent ) :
	 FixedText( pParent, WB_3DLOOK|WB_LEFT|WB_NOLABEL|WB_VCENTER )
	,m_pTabWin( pParent )
{
	DBG_CTOR(OTableWindowTitle,NULL);
	// Hintergrund- und Textfarbe setzen
	StyleSettings aSystemStyle = Application::GetSettings().GetStyleSettings();
	SetBackground(Wallpaper(Color(aSystemStyle.GetFaceColor())));
	SetTextColor(aSystemStyle.GetButtonTextColor());

	Font aFont( GetFont() );
	aFont.SetTransparent( sal_True );
	SetFont( aFont );
}

//------------------------------------------------------------------------------
OTableWindowTitle::~OTableWindowTitle()
{
	DBG_DTOR(OTableWindowTitle,NULL);
	m_pTabWin = NULL;	
}

//------------------------------------------------------------------------------
void OTableWindowTitle::GetFocus()
{
	if(m_pTabWin)
		m_pTabWin->GetFocus();
}

//------------------------------------------------------------------------------
void OTableWindowTitle::LoseFocus()
{
	m_pTabWin->LoseFocus();
}
//------------------------------------------------------------------------------
void OTableWindowTitle::RequestHelp( const HelpEvent& rHEvt )
{
	if(m_pTabWin)
	{
		String aHelpText = m_pTabWin->GetComposedName();
		if( aHelpText.Len())
		{
			// Hilfe anzeigen
			Rectangle aItemRect(Point(0,0),GetSizePixel());
			aItemRect = LogicToPixel( aItemRect );
			Point aPt = OutputToScreenPixel( aItemRect.TopLeft() );
			aItemRect.Left()   = aPt.X();
			aItemRect.Top()    = aPt.Y();
			aPt = OutputToScreenPixel( aItemRect.BottomRight() );
			aItemRect.Right()  = aPt.X();
			aItemRect.Bottom() = aPt.Y();
			if( rHEvt.GetMode() == HELPMODE_BALLOON )
				Help::ShowBalloon( this, aItemRect.Center(), aItemRect, aHelpText);
			else
				Help::ShowQuickHelp( this, aItemRect, aHelpText );
		}
	}
}

//------------------------------------------------------------------------------
void OTableWindowTitle::Command( const CommandEvent& rEvt )
{
	switch( rEvt.GetCommand() )
	{
		case COMMAND_CONTEXTMENU:
		{
			GrabFocus();
			if ( m_pTabWin )
				m_pTabWin->Command( rEvt );
			else
				Control::Command(rEvt);
		}
	}
}

//------------------------------------------------------------------------------
//void OTableWindowTitle::Paint( const Rectangle& rRect )
//{
//	//////////////////////////////////////////////////////////////////////
//	// Ausgabe des Textes in der Titelzeile
//	Rectangle aWinRect( Point(0,0), GetSizePixel() );
//	DrawText( aWinRect, GetText(), TEXT_DRAW_CENTER|TEXT_DRAW_VCENTER|TEXT_DRAW_ENDELLIPSIS );
//}

//------------------------------------------------------------------------------
void OTableWindowTitle::KeyInput( const KeyEvent& rEvt )
{
	if ( m_pTabWin )
		m_pTabWin->KeyInput( rEvt );
}

//------------------------------------------------------------------------------
void OTableWindowTitle::MouseButtonDown( const MouseEvent& rEvt )
{
	if( rEvt.IsLeft() )
	{
		if(	rEvt.GetClicks() == 2)
		{
			Size aSize(GetTextWidth(GetText()) + 20,
						m_pTabWin->GetSizePixel().Height() - m_pTabWin->GetListBox()->GetSizePixel().Height());

			aSize.Height() += (m_pTabWin->GetListBox()->GetEntryCount() + 2) * m_pTabWin->GetListBox()->GetEntryHeight();
			if(m_pTabWin->GetSizePixel() != aSize)
			{
				m_pTabWin->SetSizePixel(aSize);

				OJoinTableView* pView = static_cast<OJoinTableView*>(m_pTabWin->getTableView());
				OSL_ENSURE(pView,"No OJoinTableView!");
				const ::std::vector<OTableConnection*>* pConns = pView->getTableConnections();
				::std::for_each(pConns->begin(),
								pConns->end(),
								::std::mem_fun(&OTableConnection::RecalcLines));

				pView->InvalidateConnections();
				pView->getDesignView()->getController().setModified(sal_True);
				pView->Invalidate(INVALIDATE_NOCHILDREN);
			}
		}
		else
		{
			Point aPos = rEvt.GetPosPixel();
			aPos = OutputToScreenPixel( aPos );
			OJoinTableView* pView = static_cast<OJoinTableView*>(m_pTabWin->getTableView());
			OSL_ENSURE(pView,"No OJoinTableView!");
			pView->NotifyTitleClicked( static_cast<OTableWindow*>(GetParent()), aPos );
		}
		GrabFocus();
	}
	else
		Control::MouseButtonDown( rEvt );
}


//------------------------------------------------------------------------------
void OTableWindowTitle::DataChanged(const DataChangedEvent& rDCEvt)
{
	if (rDCEvt.GetType() == DATACHANGED_SETTINGS)
	{
		// nehmen wir den worst-case an : die Farben haben sich geaendert, also
		// mich anpassen
		StyleSettings aSystemStyle = Application::GetSettings().GetStyleSettings();
		SetBackground(Wallpaper(Color(aSystemStyle.GetFaceColor())));
		SetTextColor(aSystemStyle.GetButtonTextColor());
	}
}
// -----------------------------------------------------------------------------
void OTableWindowTitle::StateChanged( StateChangedType nType )
{
	Window::StateChanged( nType );

	if ( nType == STATE_CHANGE_ZOOM )
	{
		const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();

		Font aFont = rStyleSettings.GetGroupFont();
		if ( IsControlFont() )
			aFont.Merge( GetControlFont() );
		SetZoomedPointFont( aFont );

		Resize();
	}
}





/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2015 hanxi
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "CCEditBoxImplLinux.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)

#include "CCEditBox.h"
#include "support/ccUTF8.h"
#include <gtk/gtk.h>

// destroy dialog when lost focus
static void dialogFocusOutCallback(GtkWidget *widget, gpointer user_data)
{
    gtk_widget_destroy(widget);
}

bool LinuxInputBox(std::string &entryLine)
{
    bool didChange = false;
    GtkWidget *dialog;
    GtkWidget *entry;
    GtkWidget *contentArea;

    gtk_init(0, NULL);
    dialog = gtk_dialog_new();
    entry = gtk_entry_new();
    contentArea = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    gtk_container_add(GTK_CONTAINER(contentArea), entry);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", 0);
    // gtk_entry_set_text(GTK_ENTRY(entry), "");

    g_signal_connect(dialog, "focus-out-event", G_CALLBACK(dialogFocusOutCallback), NULL);
    gtk_window_set_keep_above(GTK_WINDOW(dialog), true);
    gtk_window_set_type_hint(GTK_WINDOW(dialog), GDK_WINDOW_TYPE_HINT_MENU);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
    gtk_widget_show_all(dialog);

    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    switch (result)
    {
    case 0:
        entryLine = gtk_entry_get_text(GTK_ENTRY(entry));
        didChange = true;
        break;
    default:
        // CCLOG("Undefined. Perhaps dialog was closed");
        break;
    }

    gtk_widget_destroy(dialog);
    while (g_main_context_iteration(NULL, false))
        ;
    return didChange;
}

NS_CC_EXT_BEGIN

CCEditBoxImpl *__createSystemEditBox(CCEditBox *pEditBox)
{
    return new CCEditBoxImplLinux(pEditBox);
}

CCEditBoxImplLinux::CCEditBoxImplLinux(CCEditBox *pEditText)
    : CCEditBoxImpl(pEditText), m_pLabel(NULL), m_pLabelPlaceHolder(NULL), m_colText(ccWHITE), m_colPlaceHolder(ccGRAY)
{
}

CCEditBoxImplLinux::~CCEditBoxImplLinux()
{
}

bool CCEditBoxImplLinux::initWithSize(const CCSize &size)
{
    //! int fontSize = getFontSizeAccordingHeightJni(size.height-12);
    m_pLabel = CCLabelTTF::create("", "", size.height - 12);
    // align the text vertically center
    m_pLabel->setAnchorPoint(ccp(0, 0.5f));
    m_pLabel->setPosition(ccp(5, size.height / 2.0f));
    m_pLabel->setColor(m_colText);
    m_pEditBox->addChild(m_pLabel);

    m_pLabelPlaceHolder = CCLabelTTF::create("", "", size.height - 12);
    // align the text vertically center
    m_pLabelPlaceHolder->setAnchorPoint(ccp(0, 0.5f));
    m_pLabelPlaceHolder->setPosition(ccp(5, size.height / 2.0f));
    m_pLabelPlaceHolder->setVisible(false);
    m_pLabelPlaceHolder->setColor(m_colPlaceHolder);
    m_pEditBox->addChild(m_pLabelPlaceHolder);

    return true;
}

void CCEditBoxImplLinux::setFont(const char *pFontName, int fontSize)
{
    if (m_pLabel != NULL)
    {
        m_pLabel->setFontName(pFontName);
        m_pLabel->setFontSize(fontSize);
    }

    if (m_pLabelPlaceHolder != NULL)
    {
        m_pLabelPlaceHolder->setFontName(pFontName);
        m_pLabelPlaceHolder->setFontSize(fontSize);
    }
}

void CCEditBoxImplLinux::setFontColor(const ccColor3B &color)
{
    m_colText = color;
    m_pLabel->setColor(color);
}

void CCEditBoxImplLinux::setPlaceholderFont(const char *pFontName, int fontSize)
{
    if (m_pLabelPlaceHolder != NULL)
    {
        m_pLabelPlaceHolder->setFontName(pFontName);
        m_pLabelPlaceHolder->setFontSize(fontSize);
    }
}

void CCEditBoxImplLinux::setPlaceholderFontColor(const ccColor3B &color)
{
    m_colPlaceHolder = color;
    m_pLabelPlaceHolder->setColor(color);
}

void CCEditBoxImplLinux::setText(const char *pText)
{
    if (pText != NULL)
    {
        m_strText = pText;

        if (m_strText.length() > 0)
        {
            m_pLabelPlaceHolder->setVisible(false);

            std::string strToShow;

            if (kEditBoxInputFlagPassword == m_eEditBoxInputFlag)
            {
                long length = cc_utf8_strlen(m_strText.c_str());
                for (long i = 0; i < length; i++)
                {
                    strToShow.append("*");
                }
            }
            else
            {
                strToShow = m_strText;
            }

            //! std::string strWithEllipsis = getStringWithEllipsisJni(strToShow.c_str(), m_EditSize.width, m_EditSize.height-12);
            //! m_pLabel->setString(strWithEllipsis.c_str());
            m_pLabel->setString(strToShow.c_str());
        }
        else
        {
            m_pLabelPlaceHolder->setVisible(true);
            m_pLabel->setString("");
        }
    }
}

const char *CCEditBoxImplLinux::getText(void)
{
    return m_strText.c_str();
}

void CCEditBoxImplLinux::setInputMode(EditBoxInputMode inputMode)
{
    m_eEditBoxInputMode = inputMode;
}

void CCEditBoxImplLinux::setInputFlag(EditBoxInputFlag inputFlag)
{
    m_eEditBoxInputFlag = inputFlag;
}

void CCEditBoxImplLinux::setReturnType(KeyboardReturnType returnType)
{
    m_eKeyboardReturnType = returnType;
}

bool CCEditBoxImplLinux::isEditing()
{
    return false;
}

void CCEditBoxImplLinux::openKeyboard()
{
    const char *text = this->getText();
    bool didChange = LinuxInputBox(m_strText);
    if (didChange)
    {
        this->setText(text);
    }
}

NS_CC_EXT_END

#endif /* #if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX) */

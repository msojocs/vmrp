 /*! @file guimenu.h
@brief 此文件介绍了menu控件的对外接口和数据类型
@author Jassmine
@version 1.0
@date 11/2006
@remarks Copyright: 2011 Spreadtrum, Incorporated. All Rights Reserved.
@details
@note
-# 11/2006      Jassmine              Creat
-# 04/2010      hua.fang        add function: SetItemVisible/SetNodeVisible


**                              SetItemGrayed
*******************************************************************************/

/*! @page page_Menu 菜单(Menu)

在Mocor平台的MMI开发过程中，Menu控件有着举足轻重的地位，它是MMI最基础最常用的控件之一，鉴于它的重要性，我们把它作为第一个控件介绍给用户。

-# @ref sec_menu_function
-# @ref sec_menu_intro
-# @ref sec_menu_grid
    -# @ref ssec_menu_grid_intro
    -# @ref ssec_menu_grid_item
    -# @ref ssec_menu_grid_item_def
    -# @ref ssec_menu_grid_layout
    -# @ref ssec_menu_grid_modify
-# @ref sec_menu_title
-# @ref sec_menu_slide
    -# @ref ssec_menu_slide_intro
    -# @ref ssec_menu_slide_item
    -# @ref ssec_menu_slide_level_1
    -# @ref ssec_menu_slide_level_2
    -# @ref ssec_menu_slide_item
    -# @ref ssec_menu_slide_layout
    -# @ref ssec_menu_slide_modify
-# @ref sec_menu_list
-# @ref sec_menu_popup
    -# @ref ssec_menu_popup_item
    -# @ref ssec_menu_popup_item_def
    -# @ref ssec_menu_popup_create
    -# @ref ssec_menu_popup_modify
-# @ref sec_menu_link
-# @ref sec_menu_function_list


@section sec_menu_function 控件的功能

菜单控件是GUI最基本的控件，在Mocor 10A以前的平台中，大部分窗口都是以Menu控件作为部件来创建的。我们把菜单分为主菜单、二级菜单、弹出菜单等三种形式。

Mocor10A中，提供了三种类型的主菜单，分别是宫格菜单、Title类型的主菜单、以及滑动菜单。

宫格菜单，用户可根据LCD Size任意定义宫格菜单显示的类型，如9宫格、12宫格、16宫格等等。宫格菜单的选项最多可以达到36项，但可视项为用户设置的宫格数，其他不可视的选项可通过按键切换焦点或触笔点击滚动条的方法变为可视。

Title类型的主菜单，它实际上是宫格菜单的延伸，因为很多小屏的手机中、或者多国语言时，如果把文字显示在每个选项的下方，限于屏幕的大小，字符串可能无法显示完整，因此设计了Title类型的主菜单，只将的选项的文字说明显示在窗口的上方。

滑动菜单是Mocor 10A新增的主菜单类型，这类主菜单的特点是，每页的显示条目固定（比如9、12、16等），页数可动态增加，但限于内存空间，应该有个最大值的。目前我们定义最大10页，足以满足用户的需求。

二级菜单的设计使菜单的操作更具有层次性，并且实现了菜单的级联效果，即可以在同一个菜单窗口中直接进入下级菜单，而不需要重新创建新的窗口。

Pop菜单是为了体现平台的多样性风格而设计的，其功能于二级菜单没有实质上的却别。但其半屏窗口的显示效果能使人机交互界面变得更加绚丽多姿。同样的Popmenu也支持多级级联的效果。

子菜单顾名思义是上述菜单的下级菜单，也可以称为级联菜单，从设计的角度来讲，子菜单的设计使得相同类型的功能能够统一到一个MMI入口当中。二级菜单、Pop菜单都可以包含子菜单，但这里请注意，主菜单是不能包含级联菜单的，后续章节介绍。子菜单与它的上级菜单同处在一个窗口中（即窗口id相同），在创建上级菜单时直接创建，其风格与主题都与上级菜单相同。

菜单控件提供了Get/Set接口来获取/设置菜单的属性及主题，例如应用通过接口函数得到光标所在位置的信息（静态菜单的当前光标group id和menu id，动态菜单的当前节点编号）。可以设置灰掉的效果。文本效果显示标题、软键盘，滚动条，菜单项文字（选中项反色显示，菜单项文字超过屏幕大小滚动显示）。

@section sec_menu_intro 菜单设计简介

-# CREATE_MENU_CTRL
@code
#define CREATE_MENU_CTRL(_GROUP_ID_, _CTRL_ID_) \
        MMK_CREATE_MENU, _GROUP_ID_, _CTRL_ID_
@endcode
    @arg _CTRL_ID_ 当前控件ID，为MMI_CONTROL_ID_E枚举类型
    @arg _GROUP_ID_ 菜单组的ID
-# @ref GUIMENU_STYLE_E
-# @ref GUIMENU_ITEM_T
-# @ref GUIMENU_GROUP_T

@note 关于菜单资源的定义:
- 菜单可以有静态菜单和动态菜单两种类型，静态菜单的资源是作为静态数据定义在平台中的，平台初始化时，注册每个模块的静态菜单资源，创建菜单控件时，根据菜单组的ID读取菜单资源。

@section sec_menu_grid 宫格主菜单

如图 2 10所示，示例所变现的是九宫格形式。所谓的九宫格菜单，它包括九个选项，以3行3列的方式排列，每个项包括要显示的图片、文本、动画等数据信息。
@image html menu_9_grid.jpg
图 2 10九宫格菜单

九宫格菜单可以扩展为12宫格菜单，以4行3列显示，当然也可以扩展成其它的排列方式。

@subsection ssec_menu_grid_intro 宫格主菜单的设计概述

- 宫格主菜单以矩阵形式显示，包含的信息有每个Item的图片、文本、当前item高亮标识等。可以显示超过一页的条目，最大条目为36个（用户可调），超过一屏时，右侧显示滚动条，可通过按键或tp多动滚动条的方式使任意一个item可见。
- 主菜单有组织功能，可通过TP长按item进入组织状态，并拖动图标进行位置更换。对于纯按键的平台，组织功能被屏蔽。

@subsection ssec_menu_grid_item 菜单项数组

数组定义在mmi_menutable.c文件中
@code
const GUIMENU_ITEM_T menu_mainmenu_icon[] =
{
    {ID_COMMUNICATION_CL,       TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},STXT_MAIN_CL,       IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_CL,       0,MENU_NULL},
    {ID_COMMUNICATION_PB,       TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},STXT_MAIN_PB,       IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_PB,       0,MENU_NULL},
    {ID_COMMUNICATION_BROWSER,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_COMMUNICATION_BROWSER,  IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_BROWSER,  0,MENU_NULL},
    {ID_MAINMENU_PLAYER,        TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},STXT_MAIN_MULTIMEDIA,       IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_PLAYER,   0,MENU_NULL},
    {ID_COMMUNICATION_MESSAGE,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_COMMUNICATION_MESSAGE,  IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_MESSAGE,  0,MENU_NULL},
    {ID_MAINMENU_TOOLS,         TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MAINMENU_ICON_APPS,     IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_TOOLS,    0,MENU_NULL},
    {ID_MAINMENU_PASTIME,       TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MAINMENU_ICON_PASTIME,  IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_GAME,     0,MENU_NULL},
    {ID_ENTERTAIMENT_FAVORITE,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_EXPLORER,               IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_FAVORITE, 0,MENU_NULL},
    {ID_MAINMENU_SETTINGS,      TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_MAINMENU_ICON_SETTINGS, IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_SET,      0,MENU_NULL},
};
@endcode

每一项表示一个菜单数据项，所包含的信息有菜单项的ID、softkey显示的文本（菜单项为当前焦点时显示）、文本、焦点图片、非焦点图片等。

@note 主菜单使用该数组显示初始化的信息，即默认的条目。当第一次开机之后进入宫格主菜单，控件读取该数组信息，然后保存到NV中，以后再次进入主菜单，控件将会读取NV中的相关信息进行显示。

@warning 这里一定注意两点：
-# 主菜单不能有级联菜单，即每个item的最后一项必须为MENU_NULL，否则，将会出现显示异常，因为在使用多层显示主菜单及二级菜单之后，级联菜单不会打开新窗口，这样多层数据不会释放，从而横显示问题，关于多层问题，请参考本文相关章节。
-# 主菜单所有的item都必须是静态数组的信息，不能动才创建主菜单Item。因为受NV空间的限制，NV中只保存了菜单Item的ID，及菜单组的group ID，显示的时候，仍然需要从它们所在的数组中读取数据。

@subsection ssec_menu_grid_item_def 菜单组的定义

@code
MENU_DEF( MENU_MAINMENU_ICON, menu_mainmenu_icon, GUIMENU_STYLE_ICON, TXT_NULL, IMAGE_NULL, IMAGE_NULL, IMAGE_NULL, GUIMENU_ITEM_NUM(menu_mainmenu_icon))
@endcode

MENU_DEF宏的参数从前往后依次为：
- 菜单组ID
- 菜单数组地址
- 菜单类型（可以看到为GUIMENU_STYLE_ICON,）
- 四个空的文本或图片（NULL表示不需要）
- 菜单数组的项数

@subsection ssec_menu_grid_layout 宫格主菜单Layout设计

宫格菜单的显示包括图标显示、文本显示等，用户需要在头文件mmitheme_menu.h中定义宫格图片的整体区域和文字显示的整体区域，例如：
@image html menu_grid_layout.jpg
图 2 11宫格菜单Layout

从上图可以看到RECT_ICON(16, 30, 223, 253)是图片显示的区域，width_icon = 32;height_icon = 40为行距和列距。控件根据这写数据信息来计算宫格菜单的每个图标的显示位置。

同样的，RECT_TEXT(10, 80, 229, 271)是文字的显示区域，width_text = 20; height_text = 72为文字的行距和列距，这样，控件就可以计算出每个选项的文字显示区域。

宫格菜单的Layout信息都定义在 GUIMENU_ICONMENU_DATA_T 结构体中。

@note
-# Title类型主菜单时，title_text_rect是有效区域，相应的item_text_display_rect数组中所有区域都是空区域。但这个数组中的区域不是用户填入的，而是控件自己计算的，用户可不必关心。相反地，如果不是Title类型的宫格菜单，那么title_text_rect就一定是一个空区域。当然这些都是控件计算出来的。
-# MMITHEME_MENUMAIN_ICON_ITEM_MAX_NUM = page_row_num× page_col_num；这是一定要注意的，否则将会出现显示错误。
-# item_width与item_height是通过MENUICON_WIDTH和MENUICON_HEIGHT获得的，这两个宏比较特殊，它们与工程的make file相关联，在工程的make file中都有类似SPECIAL_EFFECT_ICON_SIZE = 48X48 的宏定义，48X48说明图片的宽度和高度为48。之所以这样规定，是受3D效果的限制，因为开启3D效果时，底层会通过make file在系统初始化时读取这个数值进行内存分配。切记！

@subsection ssec_menu_grid_modify 如何修改九宫格菜单

九宫格菜单在特定项目中只存在一组，被用作主菜单的初始化数据资源。

修改九宫格菜单风格包括修改图片显示位置、文本显示位置、菜单的行数和列数等位置信息；以及菜单项的图片、菜单图片的高亮图片、选项对应的文本等选项信息。

如果修改位置信息，那么只需要修改mmitheme_menu.h文件中的宏定义即可，有横竖屏的，应该兼顾。

如果修改选项数据信息，只需要修改菜单的静态数组，例如menu_mainmenu_icon。

@section sec_menu_title Title类型的宫格菜单

与上一节的宫格菜单类似，Title类型的宫格菜单只是把文字显示在窗口的上方，请看下图：
@image html menu_title.jpg
图 2 12Title类型的宫格菜单

Title类型的宫格菜单，沿用了宫格菜单的所有数据结构，所不同的就是Layout的设计，如下图：
@image html menu_title_layout.jpg
图 2 13 Title类型的宫格菜单的Layout

上图中的RECT_ICON(16， 75，223，263)为图片显示区域，width_icon = 32; height_icon = 25;为图片的行距与列距。最上方的title_rect（21,36, 218, 62）（图中没画，抱歉，这里大概估算一下吧），为title文字显示区域。

@section sec_menu_slide 滑动主菜单

滑动主菜单的显示形式类似与宫格菜单，但是滑动菜单支持多页功能，图 2 14中的下方显示了页数和当前页。
@image html menu_slide.jpg
图 2 14滑动主菜单

@subsection ssec_menu_slide_intro 滑动菜单设计概述

滑动主菜单同宫格菜单类似，以矩阵形式显示，包含的信息同样有每个Item的图片、文本、当前item高亮标识等。不同的是，滑动菜单每页所显示的item是受限制的，比如九宫格类型，就限制为9个图标。

滑动菜单具有多页功能，目前设计，最大可显示10页，这应该可以满足需求了。用户可通过按键或触笔滑动切换页面。

滑动主菜单有组织功能，可通过TP长按item进入组织状态，并拖动图标进行位置更换。对于纯按键的平台，组织功能被屏蔽。

@subsection ssec_menu_slide_item 菜单项数组

在mmi_menutable.c文件中定义了滑动菜单的静态数组
-# menu_mainmenu_slide_page数组，定义滑动菜单初始化的页以及关联的页面菜单项。
@code
const GUIMENU_ITEM_T menu_mainmenu_slide_page[] =
{
    {ID_MAINMENU_SLIDE_PAGE1, TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},0, 0, 0, 0, MENU_SLIDE_COMMUNICATION},
    {ID_MAINMENU_SLIDE_PAGE2, TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},0, 0, 0, 0, MENU_SLIDE_PASTIME},
    {ID_MAINMENU_SLIDE_PAGE3, TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},0, 0, 0, 0, MENU_SLIDE_TOOLS},
    {ID_MAINMENU_SLIDE_PAGE4, TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},0, 0, 0, 0, MENU_SLIDE_PAGE4},
    {ID_MAINMENU_SLIDE_PAGE5, TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},0, 0, 0, 0, MENU_SLIDE_PAGE5},
    {ID_MAINMENU_SLIDE_PAGE6, TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},0, 0, 0, 0, MENU_SLIDE_PAGE6},
};
@endcode
-# 以关联的菜单MENU_SLIDE_COMMUNICATION为例：
@code
const GUIMENU_ITEM_T menu_slide_communication[] = // 注意，有最大数限制，最大数为宫格的最大数值
{
    // 第一行默认值：拨号盘	    通讯录	    通话记录
    {ID_COMMUNICATION_DIAL,     TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_COMM_DIALPAD,      IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_DIAL,    0,MENU_NULL},
    {ID_COMMUNICATION_PB,       TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},STXT_MAIN_PB,       IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_PB,      0,MENU_NULL},
    {ID_COMMUNICATION_CL,       TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},STXT_MAIN_CL,       IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_CL,      0,MENU_NULL},

    // 第二行默认值：蓝牙 	    信息服务 	浏览器
#ifdef BLUETOOTH_SUPPORT
	{ID_MAINMENU_BT,            TIP_NULL, {STXT_OK, TXT_NULL, STXT_RETURN}, TXT_BLUETOOTH, IMAGE_MAINMENU_ICON_SELECT,  IMAGE_MAINMENU_ICON_BT,0,MENU_NULL},
#endif
    {ID_COMMUNICATION_MESSAGE,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_COMMUNICATION_MESSAGE,  IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_MESSAGE, 0,MENU_NULL},
#ifdef BROWSER_SUPPORT
    {ID_COMMUNICATION_BROWSER,  TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_COMMUNICATION_BROWSER,  IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_BROWSER, 0,MENU_NULL},
#endif

    // 第三行默认值：Wifi/wlan	    DCD         STK
#ifdef WIFI_SUPPORT
	{ID_CONNECTION_WIFI,            TIP_NULL, {STXT_OK, TXT_NULL, STXT_RETURN}, TXT_WIFI, IMAGE_MAINMENU_ICON_SELECT,  IMAGE_MAINMENU_ICON_WIFI,0,MENU_NULL},
#endif
#ifdef DCD_SUPPORT
    {ID_TOOLS_DCD,          TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_DCD_QUICK_INFO,      IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_DCD,     0,MENU_NULL},
#endif
    {ID_COMMUNICATION_STK,      TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_COMMON_MAIN_STK,    IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_STK,     0,MENU_NULL},
};
@endcode

在这里，我们和级联菜单不期而遇，所谓的级联菜单就是当你在选种某一个菜单项的时候，打开了另一组菜单，这就是级联菜单的含义，这里我们点到为止，在后面的级联菜单一节中会作专门的介绍的。但是，冲突出来了，前文在介绍宫格菜单时，特别要求，主菜单不能设计级联菜单，这里为什么又出现级联菜单了呢？简直让人迷惑。

其实，主菜单的设计只是借用了guimenu(菜单)控件的显示流程，以及初始化菜单数据时的数组。滑动菜单所用到级联菜单是同时显示在一个窗口中的，即第一级用来显示title，第二级，才是真正的item内容。因此，这里只是借用了级联菜单的数据结构类定义滑动菜单的初始化数据罢了。

滑动主菜单的页数信息是通过menu_mainmenu_slide_page这样的数组定义的，而每页的选项数据则是通过诸如MENU_SLIDE_COMMUNICATION这样的菜单组定义出来的。

同样，这些信息只能是开机时的初始化信息，之后重新进入主菜单，所显示的信息都是从NV中毒出来的了，而且只读item id和group id。其他信息仍然需要通过静态数组查询。

例如MENU_SLIDE_COMMUNICATION和MENU_SLIDE_PASTIME中的选项可能会通过组合功能组合到一个页面内，查询时，显然它们又不在同一个数组中。

@subsection ssec_menu_slide_level_1 一级菜单组的定义

MENU_DEF( MENU_MAINMENU_SLIDE_PAGE, menu_mainmenu_slide_page, GUIMENU_STYLE_SLIDE_PAGE, TXT_NULL, IMAGE_NULL, IMAGE_NULL, IMAGE_NULL, GUIMENU_ITEM_NUM(menu_mainmenu_slide_page))

菜单组ID、菜单数组地址、菜单类型（可以看到为GUIMENU_STYLE_SLIDE_PAGE），四个空的文本或图片（NULL表示不需要）、菜单数组的项数。

@subsection ssec_menu_slide_level_2 二级菜单组定义

@code
MENU_DEF( MENU_SLIDE_COMMUNICATION, menu_slide_communication, GUIMENU_STYLE_THIRD, TXT_NULL, IMAGE_NULL, IMAGE_NULL, IMAGE_NULL, GUIMENU_ITEM_NUM(menu_slide_communication))
@endcode

菜单组ID、菜单数组地址、菜单类型（可以看到为GUIMENU_STYLE_THIRD，但这个类型在滑动主菜单中是无关紧要的，因为显示的时候用不到它），四个空的文本或图片（NULL表示不需要）、菜单数组的项数。

@subsection ssec_menu_slide_item 菜单项定义

@code
{ID_COMMUNICATION_DIAL,     TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_COMM_DIALPAD,      IMAGE_MAINMENU_ICON_SELECT,IMAGE_MAINMENU_ICON_DIAL,    0,MENU_NULL},
@endcode

包含有Item id、softkey 信息、显示文本、高亮时背景图片，item图片等有效信息。

@subsection ssec_menu_slide_layout 滑动菜单的Layout设计

如图 2 14所示，滑动菜单显示与宫格类似，所不同的是多了一个显示页面的区域，我们以page_rect （80, 274, 160, 285）表示，显然，页面标识只显示1行，因此行间距为0，只需要给出列间距就可以计算了

滑动菜单的Layout信息都定义在 GUIMENU_SLIDEMENU_DATA_T 结构体中。

@note
-# MMITHEME_MENUMAIN_SLIDE_PAGE_NUM 定义页面的最大数。
-# MMITHEME_MENUMAIN_ICON_ITEM_MAX_NUM = page_row_num× page_col_num；这是一定要注意的，否则将会出现显示错误。
-# item_width与item_height是通过MENUICON_WIDTH和MENUICON_HEIGHT获得的，这两个宏比较特殊，它们与工程的make file相关联，在工程的make file中都有类似SPECIAL_EFFECT_ICON_SIZE = 48X48 的宏定义，48X48说明图片的宽度和高度为48。之所以这样规定，是受3D效果的限制，因为开启3D效果时，底层会通过make file在系统初始化时读取这个数值进行内存分配。切记！

@subsection ssec_menu_slide_modify 如何修改滑动菜单

-# 通过修改滑动菜单的页的定义数组，来达到增加和减少初始化页面的目的。
-# 通过修改页面的下级菜单数组，来达到增加和减少页面中显示项，以及显示内容。
-# 通过修改layout调整显示的位置。

@section sec_menu_list 列表菜单

关于列表菜单的使用我们在第1篇6.4一节中已经介绍的比较详细了，请参考相关章节。

@section sec_menu_popup 弹出式菜单

弹出式菜单即PopUp Menu，如图 2 15所示，弹出式菜单的窗口没有Title；它的虚拟背景区域可以显示半透明的效果；它的菜单项可以有下一级菜单，如果菜单的某一项有下一级菜单，则以箭头表示；在一个窗口中所能显示菜单条数得最大数用户可以自由设置，如果超过最大数的时候，将会显示滚动条（如图）。
@image html menu_popup.jpg
图 2 15 弹出式菜单

@subsection ssec_menu_popup_item 菜单项数组

-# 定义POP MENU的一级菜单
@code
const GUIMENU_ITEM_T menu_test_popupmenu_table[] =
{
    {ID_POPUP_MENU_1,      TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_HELLO_TEST1,  0,    0,0,0},
    {ID_POPUP_MENU_2,      TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_HELLO_TEST2,  0,    0,0,0},
    {ID_POPUP_MENU_3,      TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_HELLO_TEST3,  0,    0,0,MENU_TEST_SUBMENU_TABLE},
};
@endcode
-# 定义POPMENU的级联菜单
@code
const GUIMENU_ITEM_T menu_test_submenu_table[] =
{
    {ID_POPUP_MENU_SUB1,      TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_HELLO_TEST1,  0,    0,0,0},
    {ID_POPUP_MENU_SUB2,      TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_HELLO_TEST2,  0,    0,0,0},
    {ID_POPUP_MENU_SUB3,      TIP_NULL,{STXT_OK,TXT_NULL,STXT_RETURN},TXT_HELLO_TEST3,  0,    0,0,0},
};
@endcode

@subsection ssec_menu_popup_item_def 菜单组定义
-# 一级菜单组宏定义
@code
MENU_DEF( MENU_TEST_POPUPMENU_TABLE, menu_test_popupmenu_table, GUIMENU_STYLE_POPUP, TXT_NULL, IMAGE_NULL, IMAGE_NULL, IMAGE_NULL, GUIMENU_ITEM_NUM(menu_test_popupmenu_table))
@endcode
-# 级联菜单组宏定义
@code
MENU_DEF( MENU_TEST_SUBMENU_TABLE, menu_test_submenu_table, GUIMENU_STYLE_POPUP, TXT_NULL, IMAGE_NULL, IMAGE_NULL, IMAGE_NULL, GUIMENU_ITEM_NUM(menu_test_popupmenu_table))
@endcode
    -# 菜单组ID
    -# 菜单数组地址
    -# 菜单类型（可以看到为GUIMENU_STYLE_POPUP）
    -# 四个空的文本或图片（NULL表示不需要）
    -# 菜单数组的项数

从数组menu_test_popupmenu_table的定义可以看出，弹出菜单的第三项有下级菜单。

@subsection ssec_menu_popup_create 菜单创建

弹出式菜单和其它菜单不同，在创建的时候，弹出式菜单不需要使用CREATE_MENU_CTRL宏，也不需要依赖于窗口Table，无论是动态创建还是静态创建都是通过接口来实现的。
-# MMIPOPMENU_CreateStatic 静态创建弹出式菜单
-# MMIPOPMENU_CreateDynamic 动态创建弹出式菜单

@subsection ssec_menu_popup_modify 修改菜单风格

弹出试菜单的风格如下：
-# 菜单显示的最大项数，该值使用一个宏定义的常量，常量名称MMIMENU_POPUP_LINE_NUM_PAGE，定义在mmimenu_position.h文件中，目前我们对于240x320以及176x220的屏幕，弹出式菜单的最大显示项数为8，而240x400的最大显示数为10。
-# 菜单的背景修改，弹出式菜单的背景分为上下两个部分，上半部分为半透的效果，使用图片IMAGE_PUBWIN_UP_BG，这是一张固定资源图片，图片的大小可以自行修改。窗口的下半部分为背景色，颜色值为MMITHEME_POPUPMENU_BG_COLOR，在创建菜单控件的时候，调用MMITHEME_GetPopupMenuTheme（mmitheme_menu.c实现）获取。
-# 菜单的其他风格信息都可以在MMITHEME_GetPopupMenuTheme接口中修改。

@section sec_menu_link 级联菜单

顾名思义，级联菜单就是一个菜单项的下一级菜单，静态定义菜单数组资源的时候可以实现设置级联菜单的菜单组ID。当执行菜单项的时候，控件根据是否有级联菜单，自动显示。如上节介绍弹出式菜单时的级联菜单设置。

@section sec_menu_function_list 关键接口介绍

-# @ref GUIMENU_GetId
-# @ref GUIMENU_InsertNode
-# MMIAPICOM_EnableGrayed

*******************************************************************************/

/*! @addtogroup menuGrp Menu
@brief menu控件
@details
菜单控件分为两大类：
- 静态菜单\n
  静态菜单的数据事先通过菜单表的形式已经确定，菜单控件分析这张菜单表解析出菜单项，
  用户不能够再动态添加和删除菜单项。\n
  现在\link #GUIMENU_STYLE_ICON \endlink、\link #GUIMENU_STYLE_OPTION_PAGE \endlink、
  \link #GUIMENU_STYLE_TITLE_ICON \endlink、\link #GUIMENU_STYLE_SLIDE_PAGE \endlink、
  \link #GUIMENU_STYLE_CUBE \endlink必须是静态菜单。
- 动态菜单\n
  动态菜单的数据是用户通过控件提供的方法来插入的。\n
  现在主要弹出式的菜单用动态菜单。
@{
*******************************************************************************/


#ifndef _GUIMENU_H_
#define _GUIMENU_H_

/*---------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "mmk_type.h"
#include "mmk_app.h"
#include "guictrl.h"
#include "guiprgbox.h"
#include "mmitheme_menu.h"
#include "mmi_link.h"
#include "ui_layer.h"
#include "guiimg.h"

/*---------------------------------------------------------------------------*

 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif

/*---------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/
/*! @def GUIMENU_ITEM_NUM
@brief 计算菜单项的个数
*/
#define GUIMENU_ITEM_NUM(_ITEMPTR)     (sizeof(_ITEMPTR)/sizeof(GUIMENU_ITEM_T))

/*! @def GUIMENU_BUTTON_MAX_NUM
@brief softkey按钮的最大个数
*/
#define GUIMENU_BUTTON_MAX_NUM          3   //softkey button num

#define GUIMENU_STACK_MAX_LEVEL         10  //from 0 to max-1

/*! @def GUIMENU_TITLE_MAX_LEN
@brief 标题的最大长度
*/
#define GUIMENU_TITLE_MAX_LEN           40  //title text max length

/*! @def GUIMENU_DYNA_NODE_MAX_NUM
@brief 调用\link GUIMENU_CreatDynamic() \endlink方法创建动态菜单时，默认的最大节点数
*/
#define GUIMENU_DYNA_NODE_MAX_NUM       50  //dynamic menu max node number

#define GUIMENU_SLIDE_PAGE_NUM           2
/*! @def GUIMENU_DYMAINMENU_STR_MAX_NUM
@brief 动态主菜单字符串长度最大值
*/
#define GUIMENU_DYMAINMENU_STR_MAX_NUM     20

#define GUIMENU_BUTTON_NUM              2

#define GUIMENU_DYMAINMENU_STR_LANG_MAX_NUM 8
/*---------------------------------------------------------------------------*
 **                         TYPE AND STRUCT                                  *
 **--------------------------------------------------------------------------*/
/*! @typedef GUIMENU_SOFTKEY_STATE_T
@brief
*/
typedef uint32 GUIMENU_SOFTKEY_STATE_T;
/*! @def GUIMENU_LEFTSK_NEED
@brief
*/
#define GUIMENU_LEFTSK_NEED     0x00000001
/*! @def GUIMENU_MIDSK_NEED
@brief
*/
#define GUIMENU_MIDSK_NEED      0x00000002
/*! @def GUIMENU_RIGHTSK_NEED
@brief
*/
#define GUIMENU_RIGHTSK_NEED    0x00000004

/*! @typedef GUIMENU_MOVE_STATE_T
@brief 通知用户如何处理菜单的move动作，该值作为MSG_NOTIFY_MENU_SET_STATUS消息的附加信息进行传递
*/
typedef uint32 GUIMENU_MOVE_STATE_T; // 定义了 菜单move时的处理
/*! @def GUIMENU_STATE_MOVE_NONE
@brief 不处理
*/
#define GUIMENU_STATE_MOVE_NONE    0x00000000
/*! @def GUIMENU_STATE_MOVE_SET
@brief 通知用户需要将菜单设置成move状态
*/
#define GUIMENU_STATE_MOVE_SET     0x00000001 // 设置组织功能
/*! @def GUIMENU_STATE_MOVE_CANCEL
@brief 通知用户取消菜单的move状态
*/
#define GUIMENU_STATE_MOVE_CANCEL  0x00000002 // cancel键处理
/*! @def GUIMENU_STATE_MOVE_OK
@brief 通知用户已经完成菜单的move动作
*/
#define GUIMENU_STATE_MOVE_OK      0x00000003 // ok键处理

// 定义菜单状态，为以后扩展使用
/*! @typedef GUIMENU_STATE_T
@brief 菜单的状态信息
*/
typedef uint32 GUIMENU_STATE_T;
/*! @def GUIMENU_STATE_ENTER
@brief 如果置该位，表示menu处于初始状态，一旦刷新后，该状态设置为FALSE
*/
#define GUIMENU_STATE_ENTER          0x00000001
/*! @def GUIMENU_STATE_CUSTOMER_TITLE
@brief 如果置该位，表示菜单不去管理标题，由用户自己控制
*/
#define GUIMENU_STATE_CUSTOMER_TITLE 0x00000002 // 用户自定义title

/*! @def GUIMENU_STATE_DRAW_SPLIT_LINE
@brief 如果置该位，表示菜单不去管理标题，由用户自己控制
*/
#define GUIMENU_STATE_DRAW_SPLIT_LINE 0x00000004 // 画分割线，主要对于列表类型的菜单，如二级菜单，popmenu


/*! @def GUIMENU_STATE_PRG_PIXEL_SCROLL
@brief 按象素滚动
*/
#define GUIMENU_STATE_PRG_PIXEL_SCROLL 0x00000008 // 按象素滚动
#if defined(PDA_UI_SUPPORT_MANIMENU_GO)
#define GO_HIGHLIGHT_DISPLAY_TIMER_OUT     10
#endif

/*! @typedef LINK_PARAMETER_T
@brief 菜单函数指针参数
*/
typedef void *DY_MENU_LINK_PARAMETER_T;
/*! @typedef NEW_ITEM_LINK_FUNC_T
@brief 动态菜单函数指针
*/
typedef void (*DY_MENU_ITEM_LINK_FUNC_T)(DY_MENU_LINK_PARAMETER_T param1, DY_MENU_LINK_PARAMETER_T param2);

/*! @typedef NEW_ITEM_LINK_DELETE_FUNC_T
@brief 动态菜单函数指针
*/
typedef BOOLEAN (*DY_MENU_ITEM_LINK_DELETE_FUNC_T)(DY_MENU_LINK_PARAMETER_T param1, DY_MENU_LINK_PARAMETER_T param2);

/* istyle 菜单文件夹使用*/
typedef void *ISTYLE_FILEFOLD_LINK_PARAMETER_T;
typedef void (*ISTYLE_FILEFOLD_ITEM_LINK_FUNC_T)(ISTYLE_FILEFOLD_LINK_PARAMETER_T param1, ISTYLE_FILEFOLD_LINK_PARAMETER_T param2);

// 定义菜单通知信息类型
typedef enum
{
    GUIMENU_NOTIFY_NONE, 
    GUIMENU_NOTIFY_DISPLAY_INIT,
    // 扩展
    GUIMENU_NOTIFY_MAX
}GUIMENU_NOTIFY_TYPE_E;

// 定义进入menu时的动画类型
typedef enum
{
    GUIMENU_ENTER_ANIM_NONE,        // 无动画 */
    GUIMENU_ENTER_ANIM_SLIDE_ICON,  // 打开主菜单时，item图标从八个方向向中间滑入 */
    // 扩展
    GUIMENU_ENTER_ANIM_MAX
}GUIMENU_ENTER_ANIM_TYPE_E;

/*! @typedef GUIMENU_FILE_DEVICE_E
@brief 数据存储路径类型
*/
typedef enum
{
    GUIMENU_DEVICE_SYSTEM,    /*!< C盘，不显示给用户，只供应用保存配置文件 */
    GUIMENU_DEVICE_UDISK,     /*!< U盘 */
    GUIMENU_DEVICE_SDCARD_1,  /*!< 卡1 */ 
    GUIMENU_DEVICE_SDCARD_2,  /*!< 卡2 */ 
    GUIMENU_DEVICE_MAX
} GUIMENU_FILE_DEVICE_E;
    

// 立方体风格时，当前处于哪种模式下
typedef enum
{
    CUBEMENU_MODE_SLIDE = 0,    // 处于slide展开模式 */
    CUBEMENU_MODE_CUBE,         // 处于立方体模式 */
    CUBEMENU_MODE_MAX
} CUBEMENU_MODE_E;

/*! @enum GUIMENU_STYLE_E
@brief 菜单风格
*/
typedef enum
{
    GUIMENU_STYLE_NULL        = 0,  /*!< 无 */
    GUIMENU_STYLE_SECOND      = 1,  /*!< 二级菜单风格，显示为列表形式，多用于由主菜单进入的二级菜单 */
    GUIMENU_STYLE_THIRD       = 2,  /*!< 已去掉此风格，其显示同GUIMENU_STYLE_SECOND */
    GUIMENU_STYLE_ICON        = 3,  /*!< 主菜单为宫格，每格文本显示在图标下面 */
    GUIMENU_STYLE_ROTATE      = 4,  /*!< 暂不支持 */
    GUIMENU_STYLE_OPTION_PAGE = 5,  /*!< 选项页风格，title处有图标来切换各个页面 */
    GUIMENU_STYLE_POPUP       = 6,  /*!< 弹出式菜单，由系统统一计算它的位置和宽度，用户不能设置 */
    GUIMENU_STYLE_POPUP_RADIO = 7,  /*!< 弹出式菜单，菜单项带有单选的radio风格 */
    GUIMENU_STYLE_POPUP_CHECK = 8,  /*!< 弹出式菜单，菜单项带有多选的check风格 */
    GUIMENU_STYLE_TITLE_ICON  = 9,  /*!< 主菜单为宫格，每格文本显示在title位置处 */
    GUIMENU_STYLE_SLIDE_PAGE  = 10, /*!< 主菜单为宫格形式，可显示多页 */
    GUIMENU_STYLE_POPUP_AUTO  = 11, /*!< 弹出式菜单，其坐标可由用户设置，且会根据文本自适应宽度 */
    GUIMENU_STYLE_POPUP_RECT  = 12, /*!< 弹出式菜单，其坐标和宽度可由用户设置，系统不会做自适应 */
    GUIMENU_STYLE_CUBE        = 13, /*!< 立方体风格 */
    GUIMENU_STYLE_TRAPEZOID   = 14, /*!< 梯形变换 */
    GUIMENU_STYLE_OPTIONS     = 15, /*!< OptionsMenu */
    GUIMENU_ISTYLE            = 16, /*!< !phone风格 */
    GUIMENU_STYLE_CRYSTAL_CUBE = 17,/*!< 水晶立方体风格 */
    GUIMENU_STYLE_CYCLONE     = 18, /*!< 龙卷风风格 */
    GUIMENU_STYLE_SPHERE      = 19, /*!< 球体 */
    GUIMENU_STYLE_CYLINDER    = 20, /*!< 圆柱体 */
    GUIMENU_STYLE_BINARY_STAR = 21, /*!< 双子星风格  */ 
    GUIMENU_STYLE_SHUTTER = 22,     /*!< 百叶窗风格  */ 
    GUIMENU_STYLE_CHARIOT = 23,     /*!< 滚咕噜咕噜滚*/
    GUIMENU_STYLE_QBTHEME = 24,     /*!< 千变主题风格*/
#ifdef PDA_UI_SUPPORT_MANIMENU_GO
/*//19套
	GUIMENU_STYLE_FLASH,
	GUIMENU_STYLE_CLASSIC,
	GUIMENU_STYLE_BINARY_STAR,
	GUIMENU_STYLE_SHUTTER,
	GUIMENU_STYLE_SPHERE,
	GUIMENU_STYLE_CYLINDER,
	GUIMENU_STYLE_SLOPE, 
	GUIMENU_STYLE_WHEEL,
	GUIMENU_STYLE_OCEANWAVE,
	GUIMENU_STYLE_PEGTOP,
	GUIMENU_STYLE_TURNTABLE,
	GUIMENU_STYLE_BOXROT,
	GUIMENU_STYLE_ICONBOXROT,
	GUIMENU_STYLE_BOOKPAGE,
	GUIMENU_STYLE_MIDROTATE,
	GUIMENU_STYLE_SMALLITEMS,
	GUIMENU_STYLE_CONSTICK,
	GUIMENU_STYLE_SYMMETRY,
	GUIMENU_STYLE_BENZITEM,
*/
	GUIMENU_STYLE_GO = 25,
#endif

    GUIMENU_STYLE_MAX
} GUIMENU_STYLE_E;

/*! @enum GUIMENU_EP_E
@brief 用于GUIMENU_STYLE_POPUP_AUTO风格下，哪个角是起点
@details
在GUIMENU_STYLE_POPUP_AUTO风格下，菜单的坐标可以由用户设置，默认情况下，这类菜单
按照左上角作为起点开始显示，但在某些情况下，按照其他的参考点则比较容易，例如，
点击左软键时，需要在左软键上方弹出菜单，那么，将左下角作为起点，菜单往上弹，
这样设置用户就比较容易理解。
*/
typedef enum
{
    GUIMENU_EP_TOP_LEFT     = 0,    /*!< 左上角为有效起点 */
    GUIMENU_EP_TOP_RIGHT    = 1,    /*!< 右上角为有效起点 */
    GUIMENU_EP_BOTTOM_LEFT  = 2,    /*!< 左下角为有效起点 */
    GUIMENU_EP_BOTTOM_RIGHT = 3     /*!< 右下角为有效起点 */
} GUIMENU_EP_E;

/*! @enum GUIMENU_MAINMENU_DATA_TYPE
@brief 动态菜单数据类型
*/
typedef enum
{   
    GUIMENU_MAINMENU_DATA_TYPE_NONE_STYLE,      /*!< 无 */
    GUIMENU_MAINMENU_DATA_TYPE_STATIC,          /*!< 静态主菜单 */
    GUIMENU_MAINMENU_DATA_TYPE_DYNAMIC,         /*!< 动态主菜单 */
    GUIMENU_MAINMENU_DATA_TYPE_FILEFOLD,
    GUIMENU_MAINMENU_DATA_TYPE_MAX     
} GUIMENU_MAINMENU_DATA_TYPE;

/*! @struct GUIMENU_BUTTON_STYLE_E
@brief Button显示类型
*/
typedef enum
{
    GUIMENU_BUTTON_STYLE_NONE,                  /*!< 不显示Button*/
    GUIMENU_BUTTON_STYLE_OK,                    /*!< 显示OK */
    GUIMENU_BUTTON_STYLE_CANCEL,                /*!< 显示Cancel */
    GUIMENU_BUTTON_STYLE_OK_CANCEL,             /*!< 显示OK, Cancel两个Button */
    GUIMENU_BUTTON_STYLE_MAX
} GUIMENU_BUTTON_STYLE_E;
#ifdef MENU_CRYSTALCUBE_SUPPORT
typedef enum
{
    GUIMENU_CRYSTALCUBE_STATE_NONE = 0,
    GUIMENU_CRYSTALCUBE_STATE_ANIM,
    GUIMENU_CRYSTALCUBE_STATE_ROTATE,
    GUIMENU_CRYSTALCUBE_STATE_DISPLAY,
    GUIMENU_CRYSTALCUBE_STATE_TP,
    GUIMENU_CRYSTALCUBE_STATE_TP_CMD_REQ,
    GUIMENU_CRYSTALCUBE_STATE_TPSCROLL,
    GUIMENU_CRYSTALCUBE_STATE_TPSCROLL_REQ,
    GUIMENU_CRYSTALCUBE_STATE_TPSCROLL_BAN,    
    GUIMENU_CRYSTALCUBE_STATE_MAX
}GUIMENU_CRYSTALCUBE_STATE_E;
#endif

#ifdef MENU_CYCLONE_SUPPORT
typedef enum
{
    GUIMENU_CYCLONE_STATE_NO_INIT = 0,
    GUIMENU_CYCLONE_STATE_IDLE,
    GUIMENU_CYCLONE_STATE_ANIM,
    GUIMENU_CYCLONE_STATE_ROTATE,
    GUIMENU_CYCLONE_STATE_HIGHLIGHT,
    GUIMENU_CYCLONE_STATE_TP_DOWN,
    GUIMENU_CYCLONE_STATE_TP_DOWN_IN_ANIM,
    GUIMENU_CYCLONE_STATE_TPSCROLL,
    GUIMENU_CYCLONE_STATE_TPSCROLL_ANIM,
    GUIMENU_CYCLONE_STATE_MAX
}GUIMENU_CYCLONE_STATE_E;
#endif


/*! @struct GUIMENU_INIT_DATA_T
@brief 菜单初始化数据
*/
typedef struct
{
    BOOLEAN                 is_static;      /*!< 是否静态菜单 */
    uint16                  node_max_num;   /*!< 菜单的最大节点数，只用于动态菜单，默认为\link #GUIMENU_DYNA_NODE_MAX_NUM \endlink */
    GUI_BOTH_RECT_T         both_rect;      /*!< 横竖屏下的坐标 */
    GUIMENU_STYLE_E         menu_style;     /*!< 菜单风格，只在动态菜单有效，静态菜单的风格在定义菜单数组时已确定 */
    MMI_MENU_GROUP_ID_T     group_id;       /*!< 菜单组id，只用于静态菜单 */
} GUIMENU_INIT_DATA_T;

// 定义通知信息
typedef struct
{
    GUIMENU_NOTIFY_TYPE_E notify_type; 
}GUIMENU_NOTIFY_INFO_T;

#if defined(PDA_UI_SUPPORT_MANIMENU_GO)
typedef struct  
{
    uint32* icon_layer_buf;//由于申请的是一整块BUF，不能用layer_info
    uint16 width;
    uint16 height;
}GO_GUIMENU_ITEM_BUF_T;

#if defined(PDA_UI_MAINMENU_SUPPORT_SELECT_ANIM)
//点击特效相关
typedef enum
{
    SELECT_EFFECT_STATIC = 0,//静止
    SELECT_EFFECT_ROUNDLIGHT, //环绕光束效果
    SELECT_EFFECT_FIRE, //火焰效果
    SELECT_EFFECT_LIGHTING,//闪电效果
    SELECT_EFFECT_RANDOM,//动态随机
    SELECT_EFFECT_MAX
}GUIMENU_SELECT_EFFECT_STYLE_E;//图标选中效果枚举
#endif
#endif

typedef enum
{
    GO_SLIDE_SUPPORT_NONE = 0,// 不支持滑动     
    GO_SLIDE_SUPPORT_ONLY_X,//支持X方向滑动
    GO_SLIDE_SUPPORT_ONLY_Y,//支持Y方向滑动
    GO_SLIDE_SUPPORT_XY,   //两个方向都支持 
    GO_SLIDE_SUPPORT_MAX
}GUIMENU_GO_SLIDE_TYPE_E;

/*! @struct  GUIMENU_ITEM_T
@brief 菜单项数据结构
*/
struct GUIMENU_ITEM_T
{
    MMI_MENU_ID_T               menu_id;                            /*!< 菜单项id */
    MMI_MENU_TIP_T              tip_id;                             /*!< 暂时不用，赋TIP_NULL */
    MMI_TEXT_ID_T               button_id[GUIMENU_BUTTON_MAX_NUM];  /*!< softkey的提示信息 */
    MMI_TEXT_ID_T               text_str_id;                        /*!< 菜单项文本ID */
    MMI_IMAGE_ID_T              select_icon_id;                     /*!< 选中框的图标ID */
    MMI_IMAGE_ID_T              unselect_icon_id;                   /*!< 菜单图标ID */
    uint32                      active_image_id;                    /*!< 获得焦点时的页切换图标ID，用于GUIMENU_STYLE_OPTION_PAGE风格下 */
    MMI_MENU_GROUP_ID_T         link_group_id;                      /*!< 子菜单的组ID */
};

/*! @struct  GUIMENU_GROUP_T
@brief  菜单组数据结构
*/
typedef struct
{
    const GUIMENU_ITEM_T        *item_ptr;          /*!< 指向菜单项的数组首地址 */
    GUIMENU_STYLE_E             menu_style;         /*!< 菜单风格 */
    MMI_TEXT_ID_T               title_string_id;    /*!< 菜单标题文本ID */
    MMI_IMAGE_ID_T              title_icon_id;      /*!< 标题栏背景图片ID */
    MMI_IMAGE_ID_T              title_numicon_id;   /*!< 暂不支持 */
    MMI_IMAGE_ID_T              title_background;   /*!< 暂不支持 */
    uint16                      item_count;         /*!< 菜单项的个数，可以通过\link #GUIMENU_ITEM_NUM \endlink计算 */
} GUIMENU_GROUP_T;

// 静态菜单项扩展信息数据结构
// 该数据结构内部使用，因为GUIMENU_ITEM_T已经对外发布，为了扩展新功能和兼容，
// 才在内部使用这个结构。
typedef struct
{
    BOOLEAN     is_grayed;                          // 是否灰显 */
    BOOLEAN     is_visible;                         // 是否可见 */
    uint16      reserved;                           // 保留位，用来4字节对齐 */
} GUIMENU_ITEM_INFO_T;

// 静态菜单组扩展信息数据结构
// 该数据结构内部使用，因为GUIMENU_GROUP_T已经对外发布，为了扩展新功能和兼容，
// 才在内部使用这个结构。
typedef struct guimenu_group_info_tag
{
    MMI_MENU_GROUP_ID_T             group_id;               // 菜单组ID */
    uint16                          visible_child_item_num; // 该菜单组中可见菜单项的个数 */
    MMI_IMAGE_ID_T                  title_icon_id;          // 菜单项图标 */
    MMI_STRING_T                    title_str;              // TITLE文本
    MMI_STRING_T                    sub_title_str;          // SUB TITLE文本
    GUIMENU_BUTTON_STYLE_E          button_style;           // button style
    struct guimenu_group_info_tag   *next_group_info_ptr;   // 指向下一个菜单组扩展信息 */
} GUIMENU_GROUP_INFO_T;

/*! @struct  GUIMENU_DYNA_ITEM_T
@brief 动态菜单项数据结构
*/
typedef struct
{
    BOOLEAN             is_grayed;      /*!< 是否灰显 */
    MMI_STRING_T        *item_text_ptr; /*!< 菜单项文本 */
    MMI_IMAGE_ID_T      select_icon_id; /*!< 菜单项图标ID */
} GUIMENU_DYNA_ITEM_T;

// 内部存储动态菜单项数据结构
typedef struct guimenu_node_tag
{
    BOOLEAN                     is_visible;                 // 是否可见 */
    BOOLEAN                     is_grayed;                  // 是否灰显 */
    GUIMENU_STYLE_E             sub_menu_style;             // 子菜单风格 */
    wchar                       *node_wstr_ptr;             // 菜单项文本 */
    uint16                      node_wstr_len;              // 菜单项文本长度 */
    uint16                      child_node_num;             // 子菜单的节点数 */
    uint16                      visible_child_node_num;     // 子菜单的可见节点数 */
    uint32                      node_id;                    // 菜单项ID */
    MMI_IMAGE_ID_T              select_icon_id;             // 菜单项图标 */
    MMI_IMAGE_ID_T              title_icon_id;              // 菜单项图标 */
    MMI_STRING_T                title_str;                  // TITLE文本
    MMI_STRING_T                sub_title_str;              // SUB TITLE文本
    GUIMENU_BUTTON_STYLE_E      button_style;               // button style
    struct guimenu_node_tag     *child_node_ptr;            // 指向子菜单 */
    struct guimenu_node_tag     *parent_node_ptr;           // 指向父菜单 */
    struct guimenu_node_tag     *neighbour_node_ptr;        // 指向兄弟菜单 */
    struct guimenu_node_tag     *next_free_node_ptr;        // 串联暂时不用的节点 */
} GUIMENU_NODE_T;

// dynamic menu node memory
typedef struct
{
    uint16              free_node_num;  //free node number
    GUIMENU_NODE_T      *free_node_ptr; //free node pointer
    GUIMENU_NODE_T      *init_mem_ptr;  //init memory pointer
} GUIMENU_NODE_MEMORY_T;

// menu stack
typedef struct
{
    uint16              first_item_index;   //display item index at the top
    uint16              cur_item_index;     //selected item index
    int16               offset_y;           //
    GUI_RECT_T          rect;               //menu rect
    GUIMENU_NODE_T      *parent_node_ptr;   //parent node
    MMI_MENU_GROUP_ID_T group_id;           //group id
} GUIMENU_STACK_T;

/*! @struct  GUIMENU_POP_SEL_INFO_T
@brief 指定菜单项数据结构
*/
typedef struct
{
    BOOLEAN                 is_static;      /*!< 是否静态菜单 */
    uint32                  node_id;        /*!< 菜单节点ID @note is_static为FALSE时有效 */
    MMI_CTRL_ID_T           ctrl_id;        /*!< 菜单控件ID */
    MMI_MENU_ID_T           menu_id;        /*!< 菜单项ID @note is_static为TRUE时有效 */
    MMI_MENU_GROUP_ID_T     group_id;       /*!< 菜单组ID @note is_static为TRUE时有效 */
} GUIMENU_POP_SEL_INFO_T;

/*! @struct  GUIMENU_CHECK_INFO_T
@brief CHECK状态的菜单项信息
*/
typedef struct
{
    uint32                  node_id;        /*!< 菜单节点ID @note 动态菜单时有效 */
    MMI_MENU_ID_T           menu_id;        /*!< 菜单项ID @note 静态菜单时有效 */
    MMI_MENU_GROUP_ID_T     group_id;       /*!< 菜单组ID @note 静态菜单时有效 */
} GUIMENU_CHECK_INFO_T;

/*! @struct  GUIMENU_TITLE_INFO_T
@brief title信息数据结构
*/
typedef struct
{
    BOOLEAN                 is_static;      /*!< 是否静态菜单 */
    uint32                  node_id;        /*!< 菜单节点ID @note is_static为FALSE时有效 */
    MMI_MENU_GROUP_ID_T     group_id;       /*!< 菜单组ID @note is_static为TRUE时有效 */
    MMI_IMAGE_ID_T          title_icon_id;  /*!< title的图片 */
    MMI_STRING_T            *title_ptr;     /*!< 指向title的指针 @note 如果为PNULL，表示忽略此项，如果指向的文本为空，表示清除title，如果指向有效文本，表示设置title */
    MMI_STRING_T            *sub_title_ptr; /*!< 指向sub title的指针 @note 如果为PNULL，表示忽略此项，如果指向的文本为空，表示清除sub title，如果指向有效文本，表示设置sub title */
} GUIMENU_TITLE_INFO_T;

/*! @struct  GUIMENU_BUTTON_INFO_T
@brief button信息数据结构
*/
typedef struct
{
    BOOLEAN                 is_static;      /*!< 是否静态菜单 */
    uint32                  node_id;        /*!< 菜单节点ID @note is_static为FALSE时有效 */
    MMI_MENU_GROUP_ID_T     group_id;       /*!< 菜单组ID @note is_static为TRUE时有效 */
    GUIMENU_BUTTON_STYLE_E  button_style;   /*!< button style */
} GUIMENU_BUTTON_INFO_T;

// 存储pop-up radio/check menu的选中信息
//  双向不循环链表结构
typedef struct guimenu_pop_sel_info_tag
{
    GUIMENU_POP_SEL_INFO_T          item_info;
    struct guimenu_pop_sel_info_tag *prev_ptr;
    struct guimenu_pop_sel_info_tag *next_ptr;
} GUIMENU_POP_SEL_ITEM_T;

// pop-up radio/check menu选中信息链表
typedef struct
{
    GUIMENU_POP_SEL_ITEM_T  *init_item_ptr;
    GUIMENU_POP_SEL_ITEM_T  *last_item_ptr;
} GUIMENU_POP_SELECT_LIST_T;

// 菜单页信息数据结构
// note 该结构在读取NV里的菜单信息时使用
typedef struct
{
    uint16                      item_total_num;     // 菜单项个数 */
    uint16                      start_item_index;   // 第一个菜单项的起始索引值 */
    MMI_LINK_NODE_T             *item_head_ptr;     // 链表头，指向第一个GUIMENULIST_ITEM_INFO_T结构 */
} GUIMENULIST_ITEM_T;

//brief  菜单项信息数据结构
//note 该结构在读取NV里的菜单信息时使用
typedef struct
{
    MMI_MENU_GROUP_ID_T         group_id;           // 菜单组ID */
    MMI_MENU_ID_T               menu_id;            // 菜单项ID */
    GUIMENULIST_ITEM_T          *link_list_ptr;     // 链表头，指向子菜单的GUIMENULIST_ITEM_T结构 */
} GUIMENULIST_ITEM_INFO_T;

//brief   主菜单配置信息数据结构
typedef union
{
    GUIMENU_ICONMENU_DATA_T icon_menu_data;         // 选项卡菜单的选项数据信息 */
    GUIMENU_OPTMENU_DATA_T opt_menu_data;           // 选项卡菜单的每个选项中的Item的显示区域信息 */
    GUIMENU_SLIDEMENU_DATA_T slide_menu_data;       // 滑动菜单的每个选项中的Item的显示区域信息 */
    GUIMENU_ISTYLE_DATA_T    istyle_menu_data;      // istyle menu data
#ifdef QBTHEME_SUPPORT
    GUIMENU_QBTHEME_DATA_T  qbtheme_menu_data;      //千变主题菜单数据以及排版 
#endif 
#ifdef MENU_CRYSTALCUBE_SUPPORT
    GUIMENU_CRYSTALCUBEMENU_DATA_T crystalcube_menu_data;
#endif
} GUIMENU_MAINMENU_U;

// OptionsMenu的item信息
typedef struct
{
    BOOLEAN         is_more_item;       // 是否是more项
    uint16          line_index;         // 行索引(注意，行是从底下开始算起)
    uint16          column_index;       // 列索引(注意，列是从右边开始算起)
    GUI_RECT_T      rect;               // 显示区域
} GUIMENU_OPTIONS_ITEM_INFO_T;

// 二级滑动菜单记录的信息
typedef struct
{
    uint16     item_index;
    GUI_RECT_T item_rect;
}GUIMENU_ITEM_POS_INFO_T;

//brief 缓存信息数据结构
//note 该结构用于缓存菜单项的信息，为加快绘制速度
typedef struct
{
    GUI_LCD_DEV_INFO icon_layer;                    // 层信息 */
    uint16 width;                                   // 宽度 */
    uint16 height;                                  // 高度 */
    BOOLEAN is_draw;
}GUIMENU_ITEM_BUF_T;

//brief 移动图标时，记录菜单项信息的数据结构
typedef struct
{
    uint16     item_index;                          // 菜单项索引值 */
    GUI_RECT_T cur_rect;                            // 当前区域 */
    GUI_RECT_T des_rect;                            // 目标区域 */
}GUIMENU_ITEM_MOVE_INFO_T;

typedef struct guitem_static_item_tag
{
    MMI_MENU_GROUP_ID_T             group_id;
    MMI_MENU_ID_T                   menu_id;
    MMI_STRING_T                    text_str;
    MMI_IMAGE_ID_T                  icon_id;
    struct guitem_static_item_tag   *next_item_ptr;
} GUIMENU_STATIC_ITEM_T;

// 立方体缓存的缩放数据
typedef struct
{
    uint8               *buf_ptr;
    uint8               *target_buf_ptr;
    uint16              width;
    uint16              height;
} MMIMENU_SCALED_PAGE_T;

typedef struct 
{
    MMIMENU_LANGUAGE_TYPE_E  language_type;                     /*!< 语言类型   */
    wchar text[GUIMENU_DYMAINMENU_STR_MAX_NUM + 1];             /*!< 字符串信息 */
}GUIMENU_MAINMENU_DYNAMIC_STRING_DATA_T;

/*! @struct  GUI_MENU_MAINMENU_DYNAMIC_DATA_T
@brief 动态菜单数据
*/
typedef struct
{
    wchar text[GUIMENU_DYMAINMENU_STR_MAX_NUM + 1];             /*!< 字符串信息 */
    uint16 text_len;                                            /*!< 字符串长度 */
    GUIMENU_MAINMENU_DYNAMIC_STRING_DATA_T  str_lang_info[GUIMENU_DYMAINMENU_STR_LANG_MAX_NUM];   /*!< 字符串语言信息 */  
    BOOLEAN has_icon;                                           /*!< 是否有图片 */
    uint8 *icon_ptr;                                            /*!< 图片信息 */
    uint16 icon_height;                                         /*!< 图片实际高度 */
    uint16 icon_width;                                          /*!< 图片实际宽度 */
    DY_MENU_LINK_PARAMETER_T param1ptr;                         /*!< link_function 参数1 ,也作为删除时标识位*/
    DY_MENU_LINK_PARAMETER_T param2ptr;                         /*!< link_function 参数2 ,也作为删除时标识位*/
    uint32 parm1_size;                                          /*!< link_function 参数1的大小 */
    uint32 parm2_size;                                          /*!< link_function 参数2的大小 */
    DY_MENU_ITEM_LINK_FUNC_T link_function_ptr;                 /*!< 打开应用程序函数指针 */
    DY_MENU_ITEM_LINK_DELETE_FUNC_T delete_function_ptr;        /*!< 删除应用程序函数指针 */ 
    MMI_IMAGE_ID_T select_icon_id;                              /*!< 高亮图片id */
    uint32 icon_datasize;                                       /*!< 图片数据大小 */
    
} GUI_MENU_MAINMENU_DYNAMIC_DATA_T;

/*! @struct  GUIMENU_MAINMENU_DYNAMIC_INFO_T
@brief 动态菜单信息
*/
typedef struct
{
    uint16 page_index;                                      /*!< 动态菜单位置信息,页码数 */
    uint16 item_index;                                      /*!< 动态菜单位置信息，序号 */
    MMI_MENU_ID_T menu_id;                                  /*!< 动态菜单menu_id,传入"0",由系统动态赋值 */
    GUI_MENU_MAINMENU_DYNAMIC_DATA_T dynamic_menu_info;     /*!< 动态菜单数据信息 */
} GUIMENU_MAINMENU_DYNAMIC_INFO_T;

/*! @struct  GUIMENU_MAINMENU_DYNAMIC_INFO_T
@brief 所有动态菜单在文件系统中的存储格式
*/
typedef struct
{
    uint32 check_info;                                  /*!< 动态菜单判断是否需要同步*/
    uint32 dy_menu_num;                                 /*!< 动态菜单个数 */
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *data_ptr;          /*!< 动态菜单数据信息 */
} GUIMENU_MAINMENU_DYNAMIC_T;

/*! @struct  GUIMENU_MAINMENU_DY_ITEM_T
@brief 动态菜单详细信息
*/
typedef struct
{
    GUIMENU_MAINMENU_DYNAMIC_INFO_T *ori_data_ptr;          /*!< 源数据 */
    GUIMENU_FILE_DEVICE_E store_dev;                        /*!< 存储位置 */
} GUIMENU_MAINMENU_DY_ITEM_T;

/* 文件夹菜单项信息*/


typedef struct
{
    uint32 menu_item_num;                                         /*   文件夹中菜单个数    */
    MMI_MENU_ID_T menu_id;
    wchar text[GUIMENU_DYMAINMENU_STR_MAX_NUM + 1];             /*!< 字符串信息 */
    uint16 text_len;                                            /*!< 字符串长度 */
    //MMI_TEXT_ID_T text_str_id;                                  /*!< 菜单项文本ID */
    //GUIMENU_ITEM_BUF_T item_buf_info[GUIMENU_FILEFOLD_MAX_ITEM_NUM];   /* 文件夹中菜单项的显示buf*/
} GUIMENU_FILEFOLD_INFO_T;

/* 所有文件夹菜单项存储库*/

#define GUIMENU_MAINMENU_MAX_FILEFOLD_NUM 25

typedef struct
{
    uint16 filefold_num; /*文件夹的个数*/
    GUIMENU_FILEFOLD_INFO_T filefold_data[GUIMENU_MAINMENU_MAX_FILEFOLD_NUM];	
}GUIMENU_FILEFOLD_T;


typedef union
{
    //静态菜单信息
    GUIMENU_ITEM_T *static_menu_ptr;
#ifdef DYNAMIC_MAINMENU_SUPPORT
    //动态菜单信息
    GUIMENU_MAINMENU_DY_ITEM_T dynamic_menu;
#endif
        //文件夹菜单信息
    GUIMENU_FILEFOLD_INFO_T * filefold_menu_ptr;

} GUI_MENU_MAINMENU_DATA_T;

struct GUIMENU_MAINMENU_INFO_T
{
    MMI_MENU_GROUP_ID_T group_id;
    MMI_MENU_ID_T       menu_id;
    uint16 menu_data_type;//菜单类型    GUIMENU_MAINMENU_DATA_TYPE_STATIC/GUIMENU_MAINMENU_DATA_TYPE_DYNAMIC
    GUI_MENU_MAINMENU_DATA_T menu_data;//菜单数据
};//主菜单数据

 #ifdef MENU_CRYSTALCUBE_SUPPORT
typedef struct
{
    GUIMENU_CRYSTALCUBE_STATE_E crystalcube_state;
    GUI_LCD_DEV_INFO crystalcube_layer_handle[MMITHEME_MENUMAIN_CRYSTALCUBE_PAGE_NUM]; //页面层SRC
    GUI_LCD_DEV_INFO crystalcube_dst_layer_handle; //页面层DST
    GUI_LCD_DEV_INFO crystalcube_bg_layer_handle; //背景层
}GUIMENU_CRYSTALCUBE_CTRL_T;
#endif
#ifdef MENU_CYCLONE_SUPPORT
typedef struct
{
    int         scroll_anim_start_angle;
    int         last_anim_stop_angle;
    uint16      item_index_backup_in_scroll_anim;    
    int16 cur_select_pos_index;
    GUIMENU_CYCLONE_STATE_E cyclone_state;
    GUI_LCD_DEV_INFO cyclone_highlight_layer_handle; //页面层DST
    GUI_LCD_DEV_INFO cyclone_dst_layer_handle; //页面层DST
    GUI_LCD_DEV_INFO cyclone_icon_layer_handle; 
    GUI_LCD_DEV_INFO cyclone_bg_layer_handle; //背景层
}GUIMENU_CYCLONE_CTRL_T;
#endif
//brief 菜单控件数据结构
typedef struct guimenu_ctrl_tag
{
    CAF_VTBL_T(IGUICTRL_T)      *vtbl_ptr;
    GUICTRL_DATA_MEMBER;

    MMI_HANDLE_T                win_handle;                             //window handle

    GUIMENU_STATE_T             menu_state;
    BOOLEAN                     is_static;                              //is static menu
    BOOLEAN                     is_focus_title;                         //title is focus for option page
    BOOLEAN                     is_set_font_color;                      //is set font color
    BOOLEAN                     is_transparent;                         //是否半透背景
    BOOLEAN                     is_need_reset_rect;                     //是否需要重设区域
    BOOLEAN                     is_active;                              // is active
    
    uint8                       item_timer_id;                          //item timer id
    uint16                      item_str_index;                         //item string start index
    uint16                      menu_level;                             //menu level
    uint16                      cur_page_index;                         //default 0,from 0 to max-1
    uint16                      cur_item_index;                         //current selected menu index. in option page style, it is also item index
    uint16                      first_item_index;                       //item index at the top
    uint16                      end_item_index;                       //item index at the top
    int16                       offset_y;                               //offset y
    int16                       offset_x;                               //offset x
    uint16                      pre_cur_item_index;                     //previous current index
    uint16                      item_total_num;                         //current total visible item number. in option page style, it is page count.

    wchar                       title_wstr[GUIMENU_TITLE_MAX_LEN + 1];    //for dynamic menu or volatile static menu title
    uint16                      title_len;                              //for dynamic menu or volatile static menu title

    GUI_COLOR_T                 font_color;                             //font color
    GUIMENU_STYLE_E             cur_style;                              //menu style
    GUIMENU_GROUP_T             *cur_group_ptr;                         //current menu group info
    MMI_MENU_GROUP_ID_T         cur_group_id;                           //current menu group id
    GUIMENU_GROUP_INFO_T        *cur_group_info_ptr;                    //current additional menu group info
    GUIMENU_GROUP_INFO_T        *root_group_info_ptr;                   //additional menu group info memory
    GUIMENU_STATIC_ITEM_T       *static_item_ptr;                       //text and image for static item

    GUIMENU_NODE_MEMORY_T       node_memory;                            //dynamic menu,node memory
    GUIMENU_NODE_T              *root_node_ptr;                         //dynamic menu,root node pointer
    GUIMENU_NODE_T              *cur_parent_node_ptr;                   //dynamic menu,current parent node pointer

    MMI_TEXT_ID_T               dyna_softkey_id[GUIMENU_BUTTON_MAX_NUM];//dynamic menu,softkey text id

    GUIMENU_STACK_T             menu_stack[GUIMENU_STACK_MAX_LEVEL];    //menu stack info
    GUIMENU_POP_SELECT_LIST_T   selected_list;                          //all selected item info only for pop-up radio/check menu

    BOOLEAN                     is_need_prgbox;                         //is need prgbox or not
    BOOLEAN                     is_revise_rect;                         //is need revise rect or not，当增加滚动条的时候，需要重新调整区域
    GUIPRGBOX_CTRL_T            *prgbox_ctrl_ptr;                       //progress box control pointer

    void                        *process_func[GUIMENU_STYLE_MAX];       //menu process function

    MMITHEME_MENU_COMMON_T      common_theme;                           //menu common theme

    MMITHEME_MAINMENU_T         main_menu_theme;                        //main menu theme
    MMITHEME_SECONDMENU_T       second_menu_theme;                      //second menu theme
    MMITHEME_POPMENU_T          pop_menu_theme;                         //pop-up menu theme
    MMITHEME_OPTIONSMENU_T      options_menu_theme;                     // options menu theme

    GUIMENU_MAINMENU_U          main_menu_data;                         //主菜单的信息
    GUIMENULIST_ITEM_T          menu_list_info;                         //menu list information
    GUIMENULIST_ITEM_T          menu_bar_info;                          //menu bar information

    BOOLEAN                     is_delete_state;                          //item是否在抖动
    BOOLEAN                     is_move_state;                          //item是否在抖动
    BOOLEAN                     is_moving;                              //item是否被拖动
    BOOLEAN                     is_empty_space;                         //tp up时，是否在item上
    BOOLEAN                     is_item_pressed;                        //tp down时，是否在item上
    BOOLEAN                     is_tp_pressed;                          //is tp down?
    BOOLEAN                     is_bar_item;                            //is bar item
    //BOOLEAN                     is_open_folder;                         //is open folder

//#ifndef RUNTIME_DRAW_SUPPORT
    BOOLEAN                    is_press_without_moving;	// for compiler
    BOOLEAN                     is_press_in_folder_rect;
    BOOLEAN                     is_opened_folder_from_bar;
    BOOLEAN                     is_temp_filefold_create;                //是否建立了临时文件夹icon
    BOOLEAN                     opened_folder_is_bar_item;
    uint16                      cur_folder_idx;                         //当前文件夹
    int16                          temp_filefold_index;                    // 文件夹形成时代index
    uint16                      opened_folder_page_index; 
    uint16                      opened_folder_item_index;   
    MMI_HANDLE_T				ctrl_foldername_handle;
    GUIMENULIST_ITEM_INFO_T*    cur_folder_ptr;
//#endif
    uint16                      tmp_cur_item_index;                     //移动的时候使用
    uint16                      tmp_first_item_index;                   //移动的时候使用
    uint16                      tmp_page_item_index;                    //移动到时候使用
    BOOLEAN                     tmp_is_bar_item;                        //移动的时候使用
    uint16                      move_item_index;                        //移动的item项，用于给idle和删除用

    uint16                      tmp_width;                              //移动的时候使用
    uint16                      tmp_height;                             //移动的时候使用
    GUI_RECT_T                  move_rect;
    
    CONTINUE_MOVE_TYPE_E 	    continue_move_type;	                    // 连续移动的类型
    uint8                       tp_timer_id;				            // 为支持TP连续移动而加的timer
    uint8                       start_move_timer_id;				    // 进入抖动状态的定时器
    uint8                       shake_icon_timer_id;				    // 进入抖动状态时，每次抖动的间隔
    uint8                       titleidx_timer_id;                      // title index显示的timer ID
    uint8                       redrew_timer_id;				        // 滑动时，slide和fling状态下，画图的timer
    uint8                       user_timer_id;				            // 应用自定义的timer

    BOOLEAN                     is_display_title;                       // 是否显示title
    BOOLEAN                     is_display_item_layer;                  // 是否画item layer的数据

    GUI_LCD_DEV_INFO            highlight_layer_handle;                 //高亮层
    GUI_LCD_DEV_INFO            icon_move_layer_handle;                 //图标移动层

    MMK_TP_SLIDE_E              slide_state;                            //current slide state
    MMK_TP_SLIDE_E              tpdown_slide_state;                     //slide state when tp down
    MMI_MENU_ID_T               delete_menu_id;                         //组织状态下使用,要删除的menu_id
    GUI_POINT_T                 slide_start_point;                      //Slide时候使用
    GUI_POINT_T                 slide_pre_point;                        //Slide时候使用
    GUI_POINT_T                 pre_tp_point;                           //上一次tp move的坐标
    GUI_LCD_DEV_INFO            page_layer_handle[GUIMENU_SLIDE_PAGE_NUM]; //页面移动层1
    GUI_LCD_DEV_INFO            titleidx_layer_handle;                  // 页面索引层
	GUI_LCD_DEV_INFO			to_idle_layer_handle;     //到主菜单图层
    GUI_LCD_DEV_INFO			delete_layer_handle;      //删除图层


    GUIMENU_SOFTKEY_STATE_T     softkey_state;                          //softkey的状态，目前只有主菜单使用

    GUIMENU_ITEM_POS_INFO_T     *item_pos_info_ptr;                    // 记录Item位置信息
    GUI_LCD_DEV_INFO            item_layer;                             // 二级菜单画item的层信息
    // end

    // auto popup menu
    uint16                      max_horz_width;                         // 菜单的横向最大宽度，用于popupmenu_auto动态计算宽度时用
    uint16                      max_vert_width;                         // 菜单的纵向最大宽度，用于popupmenu_auto动态计算宽度时用
    GUIMENU_EP_E                effective_point;                        // 弹出的基点，用于popup_auto风格的菜单
    // end

    // cube menu
    uint8                       cube_back_timer_id;				        // 触笔按下时启动timer,时间到才启动移动状态
    int16                       cur_angle;                              // 当前角度
    CUBEMENU_MODE_E             cube_mode;                              // 立方体当前的模式
    GUI_LCD_DEV_INFO            cube_layer_handle;                      // 立方体层
    BOOLEAN                     is_page_scaled;                         // 页面是否被缩放缓存
    GUI_RECT_T                  pre_cube_rect;                          // 上次立方体的区域
    MMIMENU_SCALED_PAGE_T       scaled_page[MMITHEME_MENUMAIN_SLIDE_PAGE_NUM];  // 页面缩放缓存内存
    MMIMENU_SCALED_PAGE_T       *scaled_page_ptr[2];                    // 指向当前显示的页面
    //end

    float                       fling_velocity;                         //滑动速度
    BOOLEAN                     is_first_disp;                          //是否第一次显示，在特效时使用；防止重新获得焦点时再次显示特效
    GUIMENU_ENTER_ANIM_TYPE_E   enter_anim_type;                        // 进入时的动画类型

    uint16                      pre_msg_loop_page_index;                // 每次消息循环后，focus page index
    uint16                      pre_msg_loop_item_index;                // 每次消息循环后，focus项index
    uint16                      pre_msg_loop_menu_level;                // 每次消息循环后，focus项的level

    // options menu
    GUI_LCD_DEV_INFO            popup_layer_handle;                     // 动画用
    GUI_LCD_DEV_INFO            mask_layer_handle;                      // 动画用
    GUIMENU_OPTIONS_ITEM_INFO_T *options_item_info_ptr;                 // item显示信息
    uint16                      *item_len_array_ptr;                    // 一行的长度与item的长度的比值
    uint16                      *num_in_line_ptr;                       // 每行显示的个数
    uint16                      line_count;                             // 行数
    uint16                      *temp_invisible_index_ptr;              // 暂时被隐藏的项索引
    uint16                      temp_invisible_num;                     // 临时被隐藏的项数
    uint16                      dis_num_in_options_menu;                // options一级菜单页面显示的个数

    // context menu
    GUI_RECT_T                  title_rect;                             // title区域
    GUI_RECT_T                  button_rect;                            // button区域
    GUI_RECT_T                  bg_rect;                                // item区域
    GUIMENU_BUTTON_STYLE_E      cur_button_style;                       // button风格
    MMI_HANDLE_T                button_handle[GUIMENU_BUTTON_NUM];      // button控件

    GUI_RECT_T   writeback_rect;//双buffer回写

#if defined(MENU_CRYSTALCUBE_SUPPORT)||defined(MENU_CYCLONE_SUPPORT)   
    BOOLEAN is_lsk_pressed;
    BOOLEAN is_rsk_pressed;
    BOOLEAN is_init;
    GUI_POINT_T rotate_start_point;
    union
    {
#ifdef MENU_CRYSTALCUBE_SUPPORT
        GUIMENU_CRYSTALCUBE_CTRL_T crystalcube_ctrl;
#endif
#ifdef MENU_CYCLONE_SUPPORT
        GUIMENU_CYCLONE_CTRL_T cyclone_ctrl;
#endif
    }guimenu_union;
    
#endif
#if defined(PDA_UI_SUPPORT_MANIMENU_GO)
    GUI_LCD_DEV_INFO  layer_bg; 
    GUIMENU_GO_SLIDE_TYPE_E      support_slide_type;                    //主菜单支持的滑动类型
#if defined(PDA_UI_MAINMENU_SUPPORT_SELECT_ANIM)
    uint8                       highlight_timer_id;                      // highlight显示的timer ID     
#endif
#endif
    uint8  init_item_timer_id;
} GUIMENU_CTRL_T;



//brief menu process function
typedef struct
{
    void    (*InitMenu)(GUIMENU_CTRL_T*);                                       //init menu
    void    (*DestoryMenu)(GUIMENU_CTRL_T*);                                    //destory menu
    void    (*DisplayMenu)(GUIMENU_CTRL_T*);                                    //display menu(必须实现)
    void    (*DisplaySpecialMenu)(GUIMENU_CTRL_T*);                             //display menu(处理主菜单震动拖选效果)
    void    (*ShowScrollItemStr)(GUIMENU_CTRL_T*);                              //display scroll item string
    MMI_RESULT_E    (*HandleMenuUpKey)(GUIMENU_CTRL_T*);                                //handle menu up key(必须实现)
    MMI_RESULT_E    (*HandleMenuDownKey)(GUIMENU_CTRL_T*);                              //handle menu down key(必须实现)
    BOOLEAN (*HandleMenuLeftKey)(GUIMENU_CTRL_T*,MMI_MESSAGE_ID_E);             //handle menu left key,return is handle cancel(返回是否需要处理)
    BOOLEAN (*HandleMenuRightKey)(GUIMENU_CTRL_T*,MMI_MESSAGE_ID_E);            //handle menu right key,return is handle ok(返回是否需要处理)
    BOOLEAN (*HandleMenuNumKey)(uint16,GUIMENU_CTRL_T*);                        //handle menu number key(返回是否需要处理)
    void    (*HandleMenuTpDown)(GUI_POINT_T*,GUIMENU_CTRL_T*);                  //handle menu tp down(必须实现)
    BOOLEAN (*HandleMenuTpUp)(GUI_POINT_T*,GUIMENU_CTRL_T*);                    //handle menu tp up(必须实现)(返回是否需要处理)
    void    (*HandleMenuTpMove)(GUI_POINT_T*,GUIMENU_CTRL_T*);                  //handle menu tp move(必须实现)
    void    (*HandleMenuTpScroll)(uint16,GUIMENU_CTRL_T*);                      //handle menu scroll bar tp
    void    (*AdjustFirstAndDisplay)(GUIMENU_CTRL_T*, BOOLEAN);                 //调整first_item_index索引并重新显示(非主菜单，实现)
    uint16  (*GetCurItemTop)(GUIMENU_CTRL_T*);                                  //获得当前Item的Top值(POP风格弹出二级时，实现)
    BOOLEAN (*MoveInit)(GUIMENU_CTRL_T*);                                       //移动信息初始化
    BOOLEAN (*MoveDestory)(GUIMENU_CTRL_T*);                                    //移动信息销毁

    BOOLEAN         (*HandleMenuOk)(GUIMENU_CTRL_T*, MMI_MESSAGE_ID_E);         //handle menu ok(返回mmimenu是否已经处理)
    MMI_RESULT_E    (*HandleMenuTimer)(GUIMENU_CTRL_T*, MMI_MESSAGE_ID_E, DPARAM);  //handle menu timer
    BOOLEAN         (*HandleMenuCancelKey)(GUIMENU_CTRL_T*, MMI_MESSAGE_ID_E);  //handle menu cancel(返回mmimenu是否已经处理)
    BOOLEAN         (*HandleMenuEndKey)(GUIMENU_CTRL_T*);                       //handle menu end
    BOOLEAN         (*SearchMenuId)(GUIMENU_CTRL_T*, MMI_MENU_ID_T);            // 判断指定的item id是否存在(已无用，可删)
    void            (*HandleMenuModifyRect)(GUIMENU_CTRL_T*);                   //handle menu modify rect(必须实现)
    MMI_RESULT_E    (*HandleSpecialMsg)(GUIMENU_CTRL_T*, MMI_MESSAGE_ID_E, DPARAM);                   //display menu
    void            (*GetItemPtr)(GUIMENU_CTRL_T*, uint16, uint16, GUIMENU_MAINMENU_INFO_T* );    // 获取指定页指定索引的静态item内容信息(用于主菜单)
    GUIMENULIST_ITEM_INFO_T* (*GetItemInfoPtr)(GUIMENU_CTRL_T*, uint16, uint16);// 获取指定页指定索引的静态item信息(用于主菜单)
    uint16 (*GetTotalItemNum)(GUIMENU_CTRL_T*, uint16);                         // 获取指定页的总item数(用于分页型的风格)
    uint16 (*GetLineNumPage)(GUIMENU_CTRL_T  *);                                // 获取一个有多少行(非主菜单实现)
    BOOLEAN (*ResetDisplayItem)(GUIMENU_CTRL_T *, int16 *);                     // 修正显示位置，获取相对于现在的偏移量

    void (*CalculateRect)(GUIMENU_CTRL_T *, uint16);                   // 计算menu的区域
} GUIMENU_PROCESS_T;

typedef enum
{
    GUIMENU_ITEM_IN_PAGE,  //in page
    GUIMENU_ITEM_IN_BAR,   //bar
    GUIMENU_ITEM_IN_FOLDER //in folder
}GUIMENU_ITEM_POSITION_E;

/*---------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/

/******************************************************************************/
//brief 判断是否是弹出式菜单
//author Jassmine
//param menu_ctrl_ptr
//return
//note
/******************************************************************************/
PUBLIC BOOLEAN GUIMENU_IsPopMenu(
                                 GUIMENU_CTRL_T     *menu_ctrl_ptr  //in
                                 );


/***************************************************************************//*!
@brief 获取菜单当前的选中项
@author Jassmine
@param ctrl_id [in] 控件ID
@param group_id_ptr [in/out] 指向菜单组ID的指针，不能为空
@param menu_id_ptr [in/out] 指向菜单项ID的指针，不能为空
@note 该方法只适用于静态菜单\n
获取动态菜单选中项，可以参考\link GUIMENU_GetCurNodeId() \endlink
*******************************************************************************/
void GUIMENU_GetId(
                   MMI_CTRL_ID_T        ctrl_id,
                   MMI_MENU_GROUP_ID_T  *group_id_ptr,  //group id
                   MMI_MENU_ID_T        *menu_id_ptr    //menu id
                   );


/***************************************************************************//*!
@brief 设置菜单的第一个可见项和当前选中项
@author Jassmine
@param first_item_index [in] 第一个可见项的索引值
@param cur_item_index [in] 当前选中项的索引值
@param ctrl_id [in] 控件ID
@note
-# 索引值按照菜单项的顺序，从0开始计算。
-# 如果设置的第一个可见项与当前选中项不在同一页上时，控件会自动调整第一个可见项的位置。
*******************************************************************************/
void GUIMENU_SetFirstAndSelectedItem(
                                     uint16         first_item_index,
                                     uint16         cur_item_index,
                                     MMI_CTRL_ID_T  ctrl_id
                                     );


/***************************************************************************//*!
@brief 设置菜单的标题文本
@author Jassmine
@param title_text_ptr [in] 指向标题文本的指针
@param ctrl_id [in] 控件ID
@note
*******************************************************************************/
void GUIMENU_SetMenuTitle(
                          MMI_STRING_T      *title_text_ptr,//title text
                          MMI_CTRL_ID_T     ctrl_id         //control id
                          );

/***************************************************************************//*!
@brief 设置Title的扩展方法
@author hua.fang
@param ctrl_id [in] 控件ID
@param title_info_ptr [in] Title内容
@return 是否设置成功
@note
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetMenuTitleEx(
   	                                  MMI_CTRL_ID_T             ctrl_id,        // [in]
          	                          GUIMENU_TITLE_INFO_T      *title_info_ptr // [in]
                                      );


/***************************************************************************//*!
@brief 创建动态菜单
@author Jassmine
@param rect_ptr [in] 指向控件显示区域的指针。如果是弹出式菜单的话，该值可以为PNULL。
@param win_id [in] 窗口ID
@param ctrl_id [in] 控件ID
@param menu_style [in] 控件风格
@note 该方法并没有指定节点数，默认为\link #GUIMENU_DYNA_NODE_MAX_NUM \endlink，
如果想要自定义节点数，请使用\link GUIMENU_CreatDynamicEx() \endlink。
*******************************************************************************/
void GUIMENU_CreatDynamic(
                          GUI_BOTH_RECT_T   *rect_ptr,  //may PNULL,pop-up no need rect
                          MMI_WIN_ID_T      win_id,
                          MMI_CTRL_ID_T     ctrl_id,
                          GUIMENU_STYLE_E   menu_style  //menu style
                          );


/***************************************************************************//*!
@brief 创建动态菜单
@author Jassmine
@param node_max_num [in] 最大的节点数
@param rect_ptr [in] 指向控件显示区域的指针。如果是弹出式菜单的话，该值可以为PNULL。
@param win_id [in] 窗口ID
@param ctrl_id [in] 控件ID
@param menu_style [in] 控件风格
@note
*******************************************************************************/
void GUIMENU_CreatDynamicEx(
                            uint16          node_max_num,   //in:
                            GUI_BOTH_RECT_T *rect_ptr,      //may PNULL,pop-up no need rect
                            MMI_WIN_ID_T    win_id,
                            MMI_CTRL_ID_T   ctrl_id,
                            GUIMENU_STYLE_E menu_style      //menu style
                            );


/***************************************************************************//*!
@brief 往动态菜单上插入节点
@author Jassmine
@param nodex_index [in] 节点索引值，从0开始
@param node_id [in] 用户自定义的节点ID，从1开始
@param parent_node_id [in] 父节点ID，如果是0，代表根节点
@param node_item_ptr [in] 指向菜单项信息的指针
@param ctrl_id [in] 控件ID
@note
-# 该方法只适用于动态菜单
-# nodex_index的索引值，是根据插入之前，菜单里已有菜单项的顺序来计算，从0开始
-# node_id必须从1开始，因为0代表根节点
*******************************************************************************/
void GUIMENU_InsertNode(
                        uint16                  nodex_index,    //node index,from 0
                        uint32                  node_id,        //node id,from 1
                        uint32                  parent_node_id, //parent node id
                        GUIMENU_DYNA_ITEM_T     *node_item_ptr, //node item
                        MMI_CTRL_ID_T           ctrl_id         //control id
                        );


/***************************************************************************//*!
@brief 重新设置动态菜单项的信息
@author James
@param nodex_index [in] 节点索引值，从0开始
@param node_id [in] 重新设置的用户自定义节点ID，从1开始
@param parent_node_id [in] 父节点ID，如果是0，代表根节点
@param node_text_ptr [in] 指向菜单项文本的指针
@param ctrl_id [in] 控件ID
@return
@note
-# 该方法只适用于动态菜单
-# nodex_index的索引值，是根据插入之前，菜单里已有菜单项的顺序来计算，从0开始
-# node_id必须从1开始，因为0代表根节点
*******************************************************************************/
void GUIMENU_ReplaceNode(
                         uint16          nodex_index,    //node index,from 0
                         uint32          node_id,        //node id,from 1
                         uint32          parent_node_id, //parent node id
                         MMI_STRING_T    *node_text_ptr, //node text
                         MMI_CTRL_ID_T   ctrl_id         //control id
                         );


/***************************************************************************//*!
@brief 设置动态菜单项是否灰显
@author Jassmine
@param is_grayed [in] 是否灰显
@param nodex_index [in] 节点索引值，从0开始
@param parent_node_id [in] 父节点ID，如果是0，代表根节点
@param ctrl_id [in] 控件ID
@return
@note
*******************************************************************************/
void GUIMENU_SetNodeGrayed(
                           BOOLEAN          is_grayed,      //set node grayed
                           uint16           nodex_index,    //node index,from 0
                           uint32           parent_node_id, //parent node id
                           MMI_CTRL_ID_T    ctrl_id         //control id
                           );

/*****************************************************************************/
//  Description : in dynamic menu, set node grayed
//  Global resource dependence : 
//  Author: aoke.hu
//  Note:
/*****************************************************************************/
void GUIMENU_SetNodeGrayedByNodeId(
                           BOOLEAN          is_grayed,      //set node grayed
                           uint32           node_id,                // [in]
                           uint32           parent_node_id, //parent node id
                           MMI_CTRL_ID_T    ctrl_id         //control id
                           );

/******************************************************************************/
//brief find node pointer by index
//author Jassmine
//param
//return
//note
/******************************************************************************/
GUIMENU_NODE_T* GUIMENU_FindNodeByIndex(
                                        uint16          node_index,
                                        GUIMENU_NODE_T  *parent_node_ptr
                                        );


/***************************************************************************//*!
@brief  设置softkey的显示内容
@author Jassmine
@param ctrl_id [in] 控件ID
@param left_softkey_text_id [in] 左软键文本ID
@param middle_softkey_text_id [in] 中软键文本ID
@param right_softkey_text_id [in] 右软键文本ID
@return
@note
*******************************************************************************/
void GUIMENU_SetDynamicMenuSoftkey(
                                   MMI_CTRL_ID_T    ctrl_id,
                                   MMI_TEXT_ID_T    left_softkey_text_id,
                                   MMI_TEXT_ID_T    middle_softkey_text_id,
                                   MMI_TEXT_ID_T    right_softkey_text_id
                                   );


/***************************************************************************//*!
@brief 设置菜单背景色
@author Jassmine
@param ctrl_id [in] 控件ID
@param bg_color [in] 背景色
@return
@note
*******************************************************************************/
PUBLIC void GUIMENU_SetMenuBgColor(
                                   MMI_CTRL_ID_T    ctrl_id,
                                   GUI_COLOR_T      bg_color
                                   );


/******************************************************************************/
//brief get menu font color
//author Jassmine
//param
//return
//note
/******************************************************************************/
PUBLIC GUI_COLOR_T GUIMENU_GetMenuFontColor(
                                            GUIMENU_CTRL_T  *menu_ctrl_ptr
                                            );


/***************************************************************************//*!
@brief 设置菜单的文本颜色
@author Jassmine
@param ctrl_id [in] 控件ID
@param font_color [in] 文本颜色
@return
@note
*******************************************************************************/
PUBLIC void GUIMENU_SetMenuFontColor(
                                     MMI_CTRL_ID_T  ctrl_id,
                                     GUI_COLOR_T    font_color
                                     );


/***************************************************************************//*!
@brief 设置radio或check风格的菜单项是否处于选中状态
@author Jassmine
@param is_selected [in] 是否选中
@param selected_info_ptr [in] 指定的菜单项
@return
@note
*******************************************************************************/
void GUIMENU_SetPopItemStatus(
                              BOOLEAN                   is_selected,
                              GUIMENU_POP_SEL_INFO_T    *selected_info_ptr
                              );

/***************************************************************************//*!
@brief 获取radio或check风格的菜单项是否处于选中状态
@author Jassmine
@param selected_info_ptr [in] 指定的菜单项
@return 是否处于选中状态
@note
*******************************************************************************/
BOOLEAN GUIMENU_GetPopItemStatus(
                                 GUIMENU_POP_SEL_INFO_T *selected_info_ptr
                                 );

/***************************************************************************//*!
@brief 获取radio或check风格处于选中状态的菜单项
@author hua.fang
@param ctrl_id [in] 控件ID
@param check_info_ptr [in] 选中状态的菜单项信息数组
@param array_len [in] 数组长度
@return 总共的选中个数
@note 用户如果不确定传入的数组到底应该多大，那么可首先调用该接口获得总数，然后再分配内存获取具体被选中项
*******************************************************************************/
PUBLIC uint16 GUIMENU_GetCheckInfo(
                                   MMI_CTRL_ID_T          ctrl_id,
                                   GUIMENU_CHECK_INFO_T   *check_info_ptr,
                                   uint16                 array_len
                                   );

/***************************************************************************//*!
@brief 设置菜单的显示区域
@author Jassmine
@param ctrl_handle [in] 控件ID
@param rect_ptr [in] 指向显示区域的指针
@return
@note
*******************************************************************************/
PUBLIC void GUIMENU_SetRect(
                            MMI_HANDLE_T    ctrl_handle,
                            GUI_RECT_T      *rect_ptr
                            );


/***************************************************************************//*!
@brief 获取当前选中项的ID
@author Jassmine
@param ctrl_id [in] 控件ID
@return 选中项的ID
@note 该方法只适用于动态菜单\n
静态菜单获取当前选中项，参考\link GUIMENU_GetId() \endlink
*******************************************************************************/
uint32 GUIMENU_GetCurNodeId(
                            MMI_CTRL_ID_T   ctrl_id //control id
                            );


/***************************************************************************//*!
@brief  设置菜单是否处于移动状态
@author xiaoqing.lu
@param ctrl_id [in] 控件ID
@param is_move [in] 是否处于移动状态
@return 是否设置成功
@note
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetMenuMoveState(
                                        MMI_CTRL_ID_T  ctrl_id,
                                        BOOLEAN		is_move
                                        );


/******************************************************************************/
//brief start timer
//author xiaoqing.lu
//param
//return
//note
/******************************************************************************/
PUBLIC void GUIMENU_StartAutoSwitchTimer(
                                         GUIMENU_CTRL_T *menu_ctrl_ptr
                                         );


/******************************************************************************/
//brief stop timer
//author xiaoqing.lu
//param
//return
//note
/******************************************************************************/
PUBLIC void GUIMENU_StopAutoSwitchTimer(
                                        GUIMENU_CTRL_T *menu_ctrl_ptr
                                        );


/******************************************************************************/
//brief  start cube back timer
//author hua.fang
//param
//return
//note
/******************************************************************************/
PUBLIC void GUIMENU_StartCubeBackTimer(
                                       GUIMENU_CTRL_T *menu_ctrl_ptr
                                       );


/******************************************************************************/
//brief stop cube back timer
//author hua.fang
//param
//return
//note
/******************************************************************************/
PUBLIC void GUIMENU_StopCubeBackTimer(
                                      GUIMENU_CTRL_T *menu_ctrl_ptr
                                      );


/******************************************************************************/
//brief start timer
//author xiaoqing.lu
//param
//return
//note
/******************************************************************************/
PUBLIC void GUIMENU_StartMoveTimer(
                                   GUIMENU_CTRL_T *menu_ctrl_ptr
                                   );


/******************************************************************************/
//brief stop timer
//author xiaoqing.lu
//param
//return
//note
/******************************************************************************/
PUBLIC void GUIMENU_StopMoveTimer(
                                  GUIMENU_CTRL_T *menu_ctrl_ptr
                                  );



/***************************************************************************//*!
@brief 设置菜单项是否灰显
@author hua.fang
@param ctrl_id [in] 控件ID
@param group_id [in] 菜单组ID
@param menu_id [in] 菜单项ID
@param is_grayed [in] 是否灰显
@return 是否设置成功
@note 该方法只适用于静态菜单
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetItemGrayed(
                                     MMI_CTRL_ID_T        ctrl_id,    // [in]
                                     MMI_MENU_GROUP_ID_T  group_id,   // [in]
                                     MMI_MENU_ID_T        menu_id,    // [in]
                                     BOOLEAN              is_grayed   // [in]
                                     );


/***************************************************************************//*!
@brief 设置菜单项是否可见
@author hua.fang
@param ctrl_id [in] 控件ID
@param nodex_index [in] 节点索引值，从0开始
@param parent_node_id [in] 父节点ID，如果是0，代表根节点
@param is_visible [in] 是否可见
@return 是否设置成功
@note 该方法只适用于动态菜单，静态菜单可调用\link GUIMENU_SetItemVisible() \endlink
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetNodeVisible(
                                      MMI_CTRL_ID_T ctrl_id,        // [in]
                                      uint16        nodex_index,    // [in] node index,from 0
                                      uint32        parent_node_id, // [in]
                                      BOOLEAN       is_visible      // [in]
                                      );


/***************************************************************************//*!
@brief 设置菜单项是否可见
@author hua.fang
@param ctrl_id [in] 控件ID
@param group_id [in] 菜单组ID
@param menu_id [in] 菜单项ID
@param is_visible [in] 是否可见
@return 是否设置成功
@note 该方法只适用于静态菜单，动态菜单可调用\link GUIMENU_SetNodeVisible() \endlink
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetItemVisible(
                                      MMI_CTRL_ID_T        ctrl_id,    // [in]
                                      MMI_MENU_GROUP_ID_T  group_id,   // [in]
                                      MMI_MENU_ID_T        menu_id,    // [in]
                                      BOOLEAN              is_visible  // [in]
                                      );

/*****************************************************************************/
//  Description : set item text and image for static menu
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetStaticItem(
                                     MMI_CTRL_ID_T        ctrl_id,    // [in]
                                     MMI_MENU_GROUP_ID_T  group_id,   // [in]
                                     MMI_MENU_ID_T        menu_id,    // [in]
                                     MMI_STRING_T         *text_ptr,  // [in] PNULL表示不设置文本
                                     MMI_IMAGE_ID_T       icon_id     // [in] 0或者IMAGE_NULL表示不设置图片
                                     );

/******************************************************************************/
//brief get title text
//author hua.fang
//param
//return
//note this method is for menu view
/******************************************************************************/
PUBLIC BOOLEAN GUIMENU_GetTitleText(
                                    GUIMENU_CTRL_T  *menu_ctrl_ptr, //[in]
                                    MMI_STRING_T    *title_str_ptr, //[out]
                                    MMI_STRING_T    *sub_title_str_ptr, // [out]
                                    MMI_IMAGE_ID_T  *title_icon_id_ptr  // [out]
                                    );


/******************************************************************************/
//brief get item text string, icon id, and is_grayed state
//author hua.fang
//param
//return
//note this method is for menu view
/******************************************************************************/
PUBLIC BOOLEAN GUIMENU_GetItem(
                               GUIMENU_CTRL_T    *menu_ctrl_ptr, // [in]
                               uint16            item_index,     // [in]
                               MMI_STRING_T      *text_ptr,      // [in/out]
                               MMI_IMAGE_ID_T    *icon_id_ptr,   // [in/out]
                               BOOLEAN           *is_grayed_ptr  // [in/out]
                               );


/******************************************************************************/
//brief get softkey text id
//author hua.fang
//param
//return
//note this method is for menu view
/******************************************************************************/
PUBLIC MMI_TEXT_ID_T* GUIMENU_GetSoftkeyText(
                                             GUIMENU_CTRL_T    *menu_ctrl_ptr  // [in]
                                             );


/******************************************************************************/
//brief current menu is have child menu
//author hua.fang
//param
//return
//note this method is for menu view.
/******************************************************************************/
PUBLIC BOOLEAN GUIMENU_IsExistChildMenu(                                // [ret] when has child menu
                                        GUIMENU_CTRL_T  *menu_ctrl_ptr, // [in]
                                        uint16          visible_index,  // [in]
                                        BOOLEAN         *is_grayed_ptr  // [in/out] may PNULL
                                        );


/******************************************************************************/
//brief get pop sel info
//author  hua.fang
//param
//return
//note this method is for menu view
/******************************************************************************/
PUBLIC BOOLEAN GUIMENU_GetPopSelInfo(
                                     GUIMENU_CTRL_T           *menu_ctrl_ptr, //[in]
                                     uint16                   item_index,     //[in]
                                     GUIMENU_POP_SEL_INFO_T   *item_info_ptr  //[in/out]
                                     );


/******************************************************************************/
//brief translate the visible index to absolute index in item array or list Global resource dependence :
//author hua.fang
//param
//return
//note this method is for menu view. the index is in the current display group
//   if it is option page style, it translates the page index. if you want
//  to translate the item index, please use GUIMENU_TranslateVisibleIndex
/******************************************************************************/
PUBLIC uint16 GUIMENU_VisibleIndex2AbsIndex(
                                            GUIMENU_CTRL_T *menu_ctrl_ptr, // [in]
                                            uint16 visible_index           // [in]
                                            );


/******************************************************************************/
//brief  translate the visible index in option page to absolute index
//author  hua.fang
//param
//return
//note this method is just for option page style
/******************************************************************************/
PUBLIC uint16 GUIMENU_TranslateVisibleIndex(
                                            GUIMENU_CTRL_T *menu_ctrl_ptr, // [in]
                                            uint16 visible_index           // [in]
                                            );


/******************************************************************************/
//brief get visible item count in specified option page
//author hua.fang
//param
//return
//note this method is for menu view
/******************************************************************************/
PUBLIC uint16 GUIMENU_GetItemCountInOptionPage(
                                               GUIMENU_CTRL_T *menu_ctrl_ptr,   // [in]
                                               uint16 option_page_index         // [in]
                                               );


/***************************************************************************//*!
@brief 是否处于移动状态
@author xiaoqing.lu
@param ctrl_id [in] 控件ID
@return 是否处于移动状态
@note
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_IsMoveState(
                                   MMI_CTRL_ID_T        ctrl_id
                                   );


/***************************************************************************//*!
@brief 取消移动状态
@author xiaoqing.lu
@param ctrl_id [in] 控件ID
@return 当前是否还处于移动状态
@note
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_CancelMoveIcon(
                                      MMI_CTRL_ID_T        ctrl_id
                                      );


/***************************************************************************//*!
@brief 设置背景是否透明
@author xiaoqing.lu
@param is_transparent [in] 是否透明
@param ctrl_id [in] 控件ID
@return
@note
*******************************************************************************/
PUBLIC void GUIMENU_SetHalfTransparent(
                                       BOOLEAN               is_transparent, //in:
                                       MMI_CTRL_ID_T         ctrl_id        //in:
                                       );


/***************************************************************************//*!
@brief 设置二级子菜单的风格
@author hua.fang
@param ctrl_id [in] 控件ID
@param node_id [in] 节点ID
@param style [in] 菜单风格
@return
@note
-# 该方法只适用于动态菜单
-# style的取值范围为{\link #GUIMENU_STYLE_POPUP \endlink,
   \link #GUIMENU_STYLE_POPUP_RADIO \endlink, \link #GUIMENU_STYLE_POPUP_CHECK \endlink}，
原来的风格也必须是以上几种。
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetSubMenuStyle(
                                       MMI_CTRL_ID_T      ctrl_id,      // [in]
                                       uint32             node_id,      // [in]
                                       GUIMENU_STYLE_E    style         // [in]
                                       );


/***************************************************************************//*!
@brief 获取二级子菜单的风格
@author hua.fang
@param ctrl_id [in] 控件ID
@param node_id [in] 节点ID
@return 菜单风格，如果没有二级子菜单，则返回\link #GUIMENU_STYLE_NULL \endlink
@note
*******************************************************************************/
PUBLIC GUIMENU_STYLE_E GUIMENU_GetSubMenuStyle(
                                               MMI_CTRL_ID_T      ctrl_id,  // [in]
                                               uint32             node_id   // [in]
                                               );


/******************************************************************************/
//brief start timer
//author xiaoqing.lu
//param
//return
//note
/******************************************************************************/
PUBLIC void GUIMENU_StartShakeIconTimer(
                                        GUIMENU_CTRL_T *menu_ctrl_ptr
                                        );


/******************************************************************************/
//brief stop timer
//author xiaoqing.lu
//param
//return
//note
/******************************************************************************/
PUBLIC void GUIMENU_StopShakeIconTimer(
                                       GUIMENU_CTRL_T *menu_ctrl_ptr
                                       );


/******************************************************************************/
//brief stop item text timer
//author Jassmine
//param
//return
//note
/******************************************************************************/
PUBLIC void GUIMENU_StopItemTextTimer(
                                      GUIMENU_CTRL_T     *menu_ctrl_ptr
                                      );


/******************************************************************************/
//brief stop item text timer
//author Jassmine
//param
//return
//note
/******************************************************************************/
PUBLIC void GUIMENU_StartItemTextTimer(
                                       GUIMENU_CTRL_T     *menu_ctrl_ptr
                                       );

#ifdef MRAPP_SUPPORT
/*****************************************************************************/
//  Description : in dynamic menu, replace node,携带icon变更
//  Global resource dependence : 
//  Author: sky.qgp
//  Note:
/*****************************************************************************/
void GUIMENU_ReplaceNodeEx(
							uint16          node_index,    //node index,from 0
							uint32          node_id,        //node id,from 1
							uint32          parent_node_id, //parent node id
							GUIMENU_DYNA_ITEM_T     *node_item_ptr, //node item
							MMI_CTRL_ID_T           ctrl_id         //control id
                        	);
#endif

/******************************************************************************/
//brief stop item text timer
//author Jassmine
//param
//return
//note
/******************************************************************************/
PUBLIC void GUIMENU_StopTitleIconDispTimer(
                                           GUIMENU_CTRL_T     *menu_ctrl_ptr
                                           );


/******************************************************************************/
//brief stop item text timer
//author Jassmine
//param
//return
//note
/******************************************************************************/
PUBLIC void GUIMENU_StartTitleIconDispTimer(
                                            GUIMENU_CTRL_T     *menu_ctrl_ptr
                                            );


/******************************************************************************/
//brief stop timer
//author xiaoqing.lu
//param
//return
//note
/******************************************************************************/
PUBLIC void GUIMENU_StopTPMoveTimer(
                                    GUIMENU_CTRL_T *menu_ctrl_ptr
                                    );


/******************************************************************************/
//brief start timer
//author xiaoqing.lu
//param
//return
//note
/******************************************************************************/
PUBLIC void GUIMENU_StartTPMoveTimer(
                                     GUIMENU_CTRL_T *menu_ctrl_ptr
                                     );


/***************************************************************************//*!
@brief 设置自适应菜单的最大宽度
@author hua.fang
@param ctrl_id [in] 控件ID
@param max_horz_width [in] 横屏下的最大宽度
@param max_vert_width [in] 竖屏下的最大宽度
@return 是否设置成功
@note 该方法仅在\link #GUIMENU_STYLE_POPUP_AUTO \endlink风格下才起作用
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetMaxWidth(
                                   MMI_CTRL_ID_T    ctrl_id,            // [in]
                                   uint16           max_horz_width,     // [in]
                                   uint16           max_vert_width      // [in]
                                   );


/***************************************************************************//*!
@brief 设置\link #GUIMENU_STYLE_POPUP_AUTO \endlink风格下，哪个角是起点值
@author hua.fang
@param ctrl_id [in] 控件ID
@param effective_point [in] 指定有效点
@return 是否设置成功
@note
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetEffectivePoint(
                                         MMI_CTRL_ID_T    ctrl_id,          // [in]
                                         GUIMENU_EP_E     effective_point   // [in]
                                         );


/******************************************************************************/
//brief is main menu
//author hongbo.lan
//param
//return
//note
/******************************************************************************/
PUBLIC BOOLEAN GUIMENU_IsMainMenu(
                                  GUIMENU_CTRL_T     *menu_ctrl_ptr  //in
                                  );


/******************************************************************************/
//brief get main menu font color
//author hongbo.lan
//param
//return
//note
/******************************************************************************/
PUBLIC GUI_COLOR_T GUIMENU_GetMMSoftkeyFontColor(
                                                 GUIMENU_CTRL_T  *menu_ctrl_ptr
                                                 );


/***************************************************************************//*!
@brief 设置菜单的状态
@author xiaoqing.lu
@param ctrl_id [in] 控件ID
@param src_state [in] 指定状态，其值请参考GUIMENU_STATE_T
@param is_true [in] 是否打开这种状态
@return 是否设置成功
@note
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetState(
                                MMI_CTRL_ID_T    ctrl_id,
                                uint32  src_state,
                                BOOLEAN is_true
                                );



/***************************************************************************//*!
@brief 获取菜单状态是否打开
@author xiaoqing.lu
@param ctrl_id [in] 控件ID
@param src_state [in] 指定状态，其值请参考GUIMENU_STATE_T
@return 该状态是否打开
@note
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_GetState(
                                MMI_CTRL_ID_T    ctrl_id,
                                uint32  src_state
                                );


/******************************************************************************/
//brief set state
//author xiaoqing.lu
//param
//return
//note
/******************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetStatePtr(
                                   GUIMENU_CTRL_T *menu_ctrl_ptr,
                                   uint32  src_state,
                                   BOOLEAN is_true
                                   );


/******************************************************************************/
//brief get state
//author xiaoqing.lu
//param
//return
//note
/******************************************************************************/
PUBLIC BOOLEAN GUIMENU_GetStatePtr(
                                   GUIMENU_CTRL_T *menu_ctrl_ptr,
                                   uint32  src_state
                                   );

/*****************************************************************************/
//  Description : get item for static menu
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC GUIMENU_STATIC_ITEM_T *MMIMENU_GetStaticItem(
                                                    GUIMENU_CTRL_T       *menu_ctrl_ptr,    // [in]
                                                    MMI_MENU_GROUP_ID_T  group_id,          // [in]
                                                    MMI_MENU_ID_T        menu_id            // [in]
                                                    );

/*****************************************************************************/
//  Description : set current selected item
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note: 可以自动调整top值
/*****************************************************************************/
void GUIMENU_SetSelectedItem(
                             MMI_CTRL_ID_T  ctrl_id,
                             uint16         cur_item_index
                             );

/*****************************************************************************/
//  Description : stop timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StopRedrawTimer(
                                    GUIMENU_CTRL_T *menu_ctrl_ptr
                                    );

/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StartRedrawTimer(
                                     GUIMENU_CTRL_T *menu_ctrl_ptr
                                     );

/*****************************************************************************/
//  Description : get pop-up menu rect,except border
//  Global resource dependence : 
//  Author: Jassmine
//  Note:
/*****************************************************************************/
PUBLIC GUI_RECT_T GUIMENU_PopmenuAdjustMenuRect(
                                                GUIMENU_CTRL_T     *menu_ctrl_ptr  //in
                                                );

/***************************************************************************//*!
@brief 设置button的显示风格
@author hua.fang
@param ctrl_id [in] 控件ID
@param button_info_ptr [in] 指定GUIMENU_BUTTON_INFO_T的指针，请参考GUIMENU_BUTTON_INFO_T
@note 该方法只对GUIMENU_STYLE_POPUP_AUTO,GUIMENU_STYLE_POPUP_CHECK,GUIMENU_STYLE_POPUP_RADIO,GUIMENU_STYLE_POPUP_RECT有效
*******************************************************************************/
PUBLIC void GUIMENU_SetButtonStyle(
                                   MMI_CTRL_ID_T            ctrl_id,         // [in]
                                   GUIMENU_BUTTON_INFO_T    *button_info_ptr // [in]
                                   );

/***************************************************************************//*!
@brief 设置格数
@author hua.fang
@param ctrl_id [in] 控件ID
@param grid_count [in] 格数
@note GUIMENU_STYLE_OPTIONS有效
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetGridNum(
                                  MMI_CTRL_ID_T  ctrl_id,      // [in]
                                  uint16         grid_count    // [in]
                                  );

/*****************************************************************************/
//  Description : 更新下面的窗口
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_UpdatePrevWindow(
                                     GUIMENU_CTRL_T *menu_ctrl_ptr  // [in]
                                     );

/*****************************************************************************/
//  Description : 临时隐藏前几项
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN GUIMENU_SetItemVisibleByNum(
                                           GUIMENU_CTRL_T   *menu_ctrl_ptr,     // [in]
                                           uint16           num,                // [in]
                                           BOOLEAN          visible             // [in]
                                           );

/*****************************************************************************/
//  Description : 关闭菜单
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_Close(
                          GUIMENU_CTRL_T *menu_ctrl_ptr     // [in]
                          );

/***************************************************************************//*!
@brief 通过index获取node id
@author hua.fang
@param ctrl_id [in] 控件ID
@param parent_node_id [in] 父节点node id
@param index [in] index
@param node_id_ptr [out] node id
@note 该方法只对dynamic创建的菜单有效
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_GetNodeIdByIndex(
                                        MMI_CTRL_ID_T   ctrl_id,                // [in]
                                        uint32          parent_node_id,         // [in]
                                        uint16          index,                  // [in]
                                        uint32          *node_id_ptr            // [out]
                                        );

/***************************************************************************//*!
@brief 通过index获取node id
@author hua.fang
@param ctrl_id [in] 控件ID
@param parent_node_id [in] 父节点node id
@param node_id [in] node id
@param index_ptr [out] index
@note 该方法只对dynamic创建的菜单有效
*******************************************************************************/
PUBLIC BOOLEAN GUIMENU_GetIndexByNodeId(
                                        MMI_CTRL_ID_T   ctrl_id,                // [in]
                                        uint32          parent_node_id,         // [in]
                                        uint32          node_id,                // [in]
                                        uint16          *index_ptr              // [out]
                                        );

/*****************************************************************************/
//  Description : 发penok消息
//  Global resource dependence : 
//  Author: hua.fang
//  Note:
// 内部方法
/*****************************************************************************/
PUBLIC void GUIMENU_NotifyPenOK(
                                GUIMENU_CTRL_T *menu_ctrl_ptr     // [in]
                                );

#ifdef PDA_UI_SUPPORT_MANIMENU_GO
#if defined(PDA_UI_MAINMENU_SUPPORT_SELECT_ANIM)
/*****************************************************************************/
//  Description : stop Highlight timer
//  Global resource dependence : 
//  Author:
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StopHighlightIconDispTimer(
											   GUIMENU_CTRL_T     *menu_ctrl_ptr
											   );

/*****************************************************************************/
//  Description : stop item text timer
//  Global resource dependence : 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void GUIMENU_StartHighlightIconDispTimer(
												GUIMENU_CTRL_T     *menu_ctrl_ptr
												);
#endif
#endif

#if defined (IM_ENGINE_SOGOU)
GUIMENU_CTRL_T* GUIMENU_GetMenuPtr(
                                   MMI_HANDLE_T ctrl_handle
                                   );
#endif

/*---------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif

#endif

/*@}*/


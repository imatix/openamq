/************************************************************/

//Menu object creation
oCMenu=new makeCM("oCMenu") //Making the menu object. Argument: menuname
oCMenu.frames = 0

//Menu properties
oCMenu.pxBetween=20
oCMenu.fromLeft=0
oCMenu.fromTop=80
oCMenu.rows=1
oCMenu.menuPlacement="center"
oCMenu.offlineRoot=""
oCMenu.onlineRoot=""
oCMenu.resizeCheck=1
oCMenu.wait=1000
oCMenu.zIndex=0

//Background bar properties
oCMenu.useBar=1
oCMenu.barWidth="507"
oCMenu.barHeight="menu"
oCMenu.barClass="clBar"
oCMenu.barX=0
oCMenu.barY=100
oCMenu.barBorderX=0
oCMenu.barBorderY=0
oCMenu.barBorderClass=""

//Level properties - ALL properties have to be spesified in level 0
oCMenu.level[0]=new cm_makeLevel() //Add this for each new level
oCMenu.level[0].width=110
oCMenu.level[0].height=20
oCMenu.level[0].regClass="clLevel0"
oCMenu.level[0].overClass="clLevel0over"
oCMenu.level[0].borderX=1
oCMenu.level[0].borderY=1
oCMenu.level[0].borderClass="clLevel0border"
oCMenu.level[0].offsetX=0
oCMenu.level[0].offsetY=0
oCMenu.level[0].rows=0
oCMenu.level[0].arrow=0
oCMenu.level[0].arrowWidth=0
oCMenu.level[0].arrowHeight=0
oCMenu.level[0].align="bottom"

//EXAMPLE SUB LEVEL[1] PROPERTIES - You have to specify the properties you want different from LEVEL[0] - If you want all items to look the same just remove this
oCMenu.level[1]=new cm_makeLevel() //Add this for each new level (adding one to the number)
oCMenu.level[1].width=oCMenu.level[0].width-2
oCMenu.level[1].height=20
oCMenu.level[1].regClass="clLevel1"
oCMenu.level[1].overClass="clLevel1over"
oCMenu.level[1].borderX=1
oCMenu.level[1].borderY=1
oCMenu.level[1].align="right"
oCMenu.level[1].offsetX=-(oCMenu.level[0].width-2)/2+20
oCMenu.level[1].offsetY=0
oCMenu.level[1].borderClass="clLevel1border"


//EXAMPLE SUB LEVEL[2] PROPERTIES - You have to spesify the properties you want different from LEVEL[1] OR LEVEL[0] - If you want all items to look the same just remove this
oCMenu.level[2]=new cm_makeLevel() //Add this for each new level (adding one to the number)
oCMenu.level[2].width=150
oCMenu.level[2].height=20
oCMenu.level[2].offsetX=0
oCMenu.level[2].offsetY=0
oCMenu.level[2].regClass="clLevel2"
oCMenu.level[2].overClass="clLevel2over"
oCMenu.level[2].borderClass="clLevel2border"


/******************************************
Menu item creation:
myCoolMenu.makeMenu(name, parent_name, text, link, target, width, height, regImage, overImage, regClass, overClass , align, rows, nolink, onclick, onmouseover, onmouseout)
*************************************/
oCMenu.makeMenu('top0','','','','','175')

oCMenu.makeMenu('top1','','|&nbsp;Home','index_en.html','','50')

oCMenu.makeMenu('top2','','|&nbsp;Software','','','75')
    oCMenu.makeMenu('top21','top2','&nbsp;License',        'license_en.html','',   '120')
    oCMenu.makeMenu('top22','top2','&nbsp;Download',       'download_en.html','',  '120')
    oCMenu.makeMenu('top23','top2','&nbsp;Install',        'install_en.html','',   '120')
    oCMenu.makeMenu('top24','top2','&nbsp;Quick Guide',    'quickguide_en.html','','120')
    oCMenu.makeMenu('top25','top2','&nbsp;Administrators', 'administer_en.html','','120')
    oCMenu.makeMenu('top26','top2','&nbsp;Developers',     'developer_en.html','', '120')

oCMenu.makeMenu('top3','','|&nbsp;Features','','','75')
    oCMenu.makeMenu('top31','top3','&nbsp;Server',         'server_en.html','',    '120')
    oCMenu.makeMenu('top32','top3','&nbsp;File Transfers', 'filetran_en.html','',  '120')
    oCMenu.makeMenu('top33','top3','&nbsp;COM+ Client',    'complus_en.html','',   '120')
    oCMenu.makeMenu('top34','top3','&nbsp;C API library',  'capilib_en.html','',   '120')

oCMenu.makeMenu('top4','','|&nbsp;Standards','','','75')
    oCMenu.makeMenu('top41','top4','&nbsp;Protocols',      'protocols_en.html','', '120')
    oCMenu.makeMenu('top42','top4','&nbsp;Components',     'components_en.html','','120')
    oCMenu.makeMenu('top43','top4','&nbsp;Technical',      'technical_en.html','', '120')

oCMenu.makeMenu('top5','','|&nbsp;Help','', '','60')
    oCMenu.makeMenu('top51','top5','&nbsp;Support',        'support_en.html','',   '120')
    oCMenu.makeMenu('top52','top5','&nbsp;Report Bug',     'report_en.html','',    '120')
    oCMenu.makeMenu('top53','top5','&nbsp;About OpenAMQ',  'about_en.html','',     '120')

oCMenu.construct()

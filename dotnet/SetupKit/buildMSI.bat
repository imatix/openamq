@rem generate intermediate format file
candle AMQXMS.wxs
@rem build the MSI
light -out AMQXMS.msi AMQXMS.wixobj ..\build\wix\lib\wixui_mondo.wixlib -loc ..\build\wix\lib\WixUI_en-us.wxl
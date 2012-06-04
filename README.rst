+++++++
dscvd3d
+++++++
DirectShowから入力した動画をOpenCVで処理してD3Dのテクスチャ表示するサンプル

build
=====
    > premake4 vs2010

usage
=====
USBカメラをチェスボードに向けてspaceキーを押す

dependency
==========
for compiler & DirectShow
-------------------------
 * VC2010 express edition

for atl's CComPtr
-----------------
 * WinDDK-7600.16385.1

for D3D Texture
---------------
 * DirectX-June2010(9C)

for DirectShow SampleGrabber(qedit.h)
-------------------------------------
 * Platform SDK[Windows SDK for Windows Server 2008 ?](qedit.h)

for image processing
--------------------
 * OpenCV-2.4

todo
====
 * テクスチャ表示にshaderを適用する
 * チェスボードの三次元位置にポリゴンを表示してみる


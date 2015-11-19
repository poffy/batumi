# -*- mode: python -*-

block_cipher = None


a = Analysis(['XAOC Firmware Update Tool.py'],
             pathex=['flasher/'],
             binaries=None,
             datas=None,
             hiddenimports=[],
             hookspath=None,
             runtime_hooks=None,
             excludes=None,
             win_no_prefer_redirects=None,
             win_private_assemblies=None,
             cipher=block_cipher)
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          a.binaries,
          a.zipfiles,
          a.datas,
          name='XAOC Firmware Update Tool',
          debug=False,
          strip=None,
          upx=True,
          console=False , icon='flasher/icon.icns')
app = BUNDLE(exe,
             name='XAOC Firmware Update Tool.app',
             icon='flasher/icon.icns',
             bundle_identifier=None)

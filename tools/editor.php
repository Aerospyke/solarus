<?php

header("Content-type: application/x-java-jnlp-file");
echo '
<?xml version="1.0" encoding="utf-8"?>
<jnlp spec="1.0+" codebase="http://zsdx.zelda-solarus.com/editor" href="editor.jnlp">
  <information>
    <title>ZSDX Editor</title>
    <vendor>Christopho</vendor>
    <homepage href="http://www.zelda-solarus.com" />
    <description>Map editor for Zelda: Mystery of Solarus DX</description>
    <offline-allowed />
    <icon href="icon.gif" />
  </information>
  <security>
    <all-permissions />
  </security>
  <resources>
    <j2se version="1.6" />
    <jar href="zsdx-editor.jar" />
    <jar href="ini4j.jar" />
  </resources>
  <application-desc main-class="zsdx.MapEditor" />
</jnlp>
';
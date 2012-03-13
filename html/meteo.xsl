<?xml version="1.0"?>

<!--

	meteo.xsl	XSL style sheet to convert the XML content files into
			HTML files

	(c) 2004 Dr. Andreas Mueller, Beratung und Entwicklung
	$Id: meteo.xsl,v 1.1 2004/02/24 23:33:21 afm Exp $
  -->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html" encoding="ASCII"/>

<xsl:template name="menutext">
  <xsl:param name="name">none</xsl:param>
  <xsl:choose>
    <xsl:when test="$name = 'about'">About</xsl:when>
    <xsl:when test="$name = 'news'">News</xsl:when>
    <xsl:when test="$name = 'install'">Install</xsl:when>
    <xsl:when test="$name = 'config'">Configure</xsl:when>
    <xsl:when test="$name = 'util'">Utilities</xsl:when>
    <xsl:when test="$name = 'prereq'">Prerequisites</xsl:when>
    <xsl:when test="$name = 'ack'">Acknowledgments</xsl:when>
    <xsl:when test="$name = 'gallery'">Gallery</xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="menuitem">
  <xsl:param name="name">none</xsl:param>
  <a><xsl:attribute name="href"><xsl:value-of select="$name"/>.html</xsl:attribute>
    <div>
      <xsl:attribute name="class">
	<xsl:choose>
	  <xsl:when test="$name = ./@name">menuitemselected</xsl:when>
	  <xsl:otherwise>menuitem</xsl:otherwise>
	</xsl:choose>
      </xsl:attribute>
      <xsl:call-template name="menutext">
	<xsl:with-param name="name"><xsl:value-of select="$name"/></xsl:with-param>
      </xsl:call-template>
    </div>
  </a>
</xsl:template>

<xsl:template match="page">
  <html>
  <head>
  <title>Meteo</title>
  <link href="meteo.css" type="text/css" rel="stylesheet"/>
  </head>
  <body>
  <div id="titel"></div>
  <div id="menu">

    <xsl:call-template name="menuitem">
      <xsl:with-param name="name">about</xsl:with-param>
    </xsl:call-template>

    <xsl:call-template name="menuitem">
      <xsl:with-param name="name">news</xsl:with-param>
    </xsl:call-template>

    <xsl:call-template name="menuitem">
      <xsl:with-param name="name">gallery</xsl:with-param>
    </xsl:call-template>

    <xsl:call-template name="menuitem">
      <xsl:with-param name="name">install</xsl:with-param>
    </xsl:call-template>

    <xsl:call-template name="menuitem">
      <xsl:with-param name="name">config</xsl:with-param>
    </xsl:call-template>

    <xsl:call-template name="menuitem">
      <xsl:with-param name="name">util</xsl:with-param>
    </xsl:call-template>

    <xsl:call-template name="menuitem">
      <xsl:with-param name="name">prereq</xsl:with-param>
    </xsl:call-template>

    <xsl:call-template name="menuitem">
      <xsl:with-param name="name">ack</xsl:with-param>
    </xsl:call-template>

  </div>
  <div id="content">
    <xsl:copy-of select="./*"/>
  </div>
  <div id="footer">
  &#xA9; 2004 <a href="mailto:afm@othello.ch">Dr. Andreas M&#xFC;ller</a>,
   <a href="http://www.othello.ch">Beratung und Entwicklung</a>
  </div>
  </body>
  </html>
</xsl:template>

</xsl:stylesheet>

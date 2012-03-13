<?xml version="1.0"?>

<!--

	gallery.xsl	XSL style sheet to convert the XML registration
			files to HTML

	(c) 2004 Dr. Andreas Mueller, Beratung und Entwicklung
	$Id: gallery.xsl,v 1.1 2007/08/29 21:03:20 afm Exp $
  -->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" omit-xml-declaration="yes"/>

<xsl:template match="gallery">
  <table>
    <xsl:apply-templates/>
  </table>
</xsl:template>

<xsl:template match="meteo-registration">
  <xsl:apply-templates/>
  <xsl:if test="count(following-sibling::*) > 0">
    <tr colspan="2"><td>&#xA0;</td></tr>
  </xsl:if>
</xsl:template>

<xsl:template match="name">
  <tr>
    <td><strong>Name:</strong></td>
    <td><strong><xsl:apply-templates/></strong></td>
  </tr>
</xsl:template>

<xsl:template match="location">
  <tr>
    <td valign="top"><strong>Location:</strong></td>
    <td valign="top"><xsl:apply-templates/></td>
  </tr>
</xsl:template>

<xsl:template name="abs">
  <xsl:choose>
    <xsl:when test="number(.) &lt; 0">
      <xsl:value-of select="-number(.)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="number(.)"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="longitude">
  <xsl:call-template name="abs"/>
  <xsl:choose>
    <xsl:when test="number(.) &lt; 0">W</xsl:when>
    <xsl:when test="number(.) &gt; 0">E</xsl:when>
  </xsl:choose>,
</xsl:template>

<xsl:template match="latitude">
  <xsl:call-template name="abs"/>
  <xsl:choose>
    <xsl:when test="number(.) &gt; 0">N</xsl:when>
    <xsl:when test="number(.) &lt; 0">S</xsl:when>
  </xsl:choose>,
</xsl:template>

<xsl:template match="altitude">
  <xsl:apply-templates/>m
</xsl:template>

<xsl:template match="city">
  <xsl:apply-templates/>,
</xsl:template>

<xsl:template match="state">
  <xsl:apply-templates/>,
</xsl:template>

<xsl:template match="country">
  <xsl:apply-templates/>,
</xsl:template>

<xsl:template match="comments">
  <tr>
    <td valign="top"><strong>Comments:</strong></td>
    <td valign="top"><xsl:apply-templates/></td>
  </tr>
</xsl:template>

<xsl:template match="contact">
  <tr>
    <td><strong>Contact:</strong></td>
    <td><xsl:apply-templates/></td>
  </tr>
</xsl:template>

<xsl:template match="url">
  <tr>
    <td><strong>Web:</strong></td>
    <td><a><xsl:attribute name="href" target="_new"><xsl:value-of select="."/></xsl:attribute><xsl:apply-templates/></a></td>
  </tr>
</xsl:template>

<xsl:template match="browser-url">
  <tr>
    <td><strong>Browser:</strong></td>
    <td><a><xsl:attribute name="href" target="_new"><xsl:value-of select="."/></xsl:attribute><xsl:value-of select="."/></a></td>
  </tr>
</xsl:template>

<xsl:template match="info-url">
  <tr>
    <td><strong>Additional&#xA0;info:</strong></td>
    <td><a><xsl:attribute name="href" target="_new"><xsl:value-of select="."/></xsl:attribute><xsl:value-of select="."/></a></td>
  </tr>
</xsl:template>

<xsl:template match="station-type">
  <tr>
    <td><strong>Station&#xA0;type:</strong></td>
    <td><xsl:apply-templates/></td>
  </tr>
</xsl:template>

<xsl:template match="connection">
  <tr>
    <td><strong>Connection:</strong></td>
    <td><xsl:apply-templates/></td>
  </tr>
</xsl:template>

<xsl:template match="additional-sensors">
  <tr>
    <td><strong>Additional&#xA0;sensors:</strong></td>
    <td><xsl:apply-templates/></td>
  </tr>
</xsl:template>

</xsl:stylesheet>

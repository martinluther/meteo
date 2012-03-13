<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html" encoding="ASCII"/>

<xsl:template match="meteomap">
<html>
<head>
<title>Current <xsl:value-of select="./@station"/> meteo data</title>
</head>
<body>
<h1>Current <xsl:value-of select="./@station"/> meteo data</h1>
<p>Last update: <xsl:value-of select="./@timestamp"/></p>
<table>
<xsl:apply-templates select="data"/>
</table>
</body>
</html>
</xsl:template>

<xsl:template match="data">
  <tr>
    <td><xsl:value-of select="./@name"/></td>
    <td><xsl:value-of select="./@value"/>
    <xsl:value-of select="./@unit"/></td>
  </tr>
</xsl:template>

</xsl:stylesheet>

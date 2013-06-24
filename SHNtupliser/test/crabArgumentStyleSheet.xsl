<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
        <xsl:output method="text"/>

        <xsl:template match="Arguments">
                <xsl:apply-templates select="Job"/>
        </xsl:template>

        <xsl:template match="Job">
                <xsl:for-each select="@JobID">
                        <xsl:value-of select="."/>
                        <xsl:value-of select="'::'"/>
                </xsl:for-each>
                <xsl:for-each select="@Lumis">
                        <xsl:value-of select="."/>
                        <xsl:value-of select="'::'"/>
                </xsl:for-each>
                 <xsl:for-each select="@InputFiles">
                        <xsl:value-of select="."/>
                        <xsl:value-of select="'::'"/>
                </xsl:for-each> 
		<xsl:for-each select="@MaxEvents">
                        <xsl:value-of select="."/>
                        <xsl:value-of select="'::'"/>
                </xsl:for-each> 
        	<xsl:for-each select="@SkipEvents">
                        <xsl:value-of select="."/>
                        <xsl:value-of select="'::'"/>
                </xsl:for-each> 
                <xsl:text>&#10;</xsl:text>
        </xsl:template> 
</xsl:stylesheet>
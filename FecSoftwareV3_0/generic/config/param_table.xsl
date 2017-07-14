<!--HTML presentation of uHal XML address table-->
<xsl:stylesheet version = '1.0' 
     xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>

<xsl:template match="/"> 
<html><body>
<H1>Table of parameters</H1>
        <table border="1"><tr><th>Address</th><th>Name</th><th>Mask</th><th>Permission</th><th>Description</th></tr>
          <xsl:apply-templates />
        </table>
</body></html> 
</xsl:template>

<xsl:template match="/node/node" priority="1"><!--First level nodes-->
        <tr>
	<td><strong><xsl:value-of select="@address"/></strong></td><td><strong><xsl:value-of select="@id"/></strong></td>
        <td><strong><xsl:value-of select="@mask"/></strong></td><td align="center"><xsl:value-of select="@permission"/></td>
        <td><xsl:value-of select="@description"/></td>
        </tr>
	<xsl:apply-templates />
</xsl:template>

<xsl:template match="node[@address and ancestor::node/@address and node/@address]" priority="2"><!--intermediate nodes-->
        <tr>
	<td><xsl:value-of select="substring('-------',0,count(ancestor::*))"/>word <xsl:value-of select="@address"/></td><td><xsl:value-of select="@id"/></td>
        <td><xsl:value-of select="@mask"/></td><td align="center"><xsl:value-of select="@permission"/></td><td><xsl:value-of select="@description"/></td>
        </tr>
	<xsl:apply-templates />
</xsl:template>

<xsl:template match="node[@address and ancestor::node/@address and not(node)]" ><!--Final leaves without bit table-->
        <tr>
	<td><xsl:value-of select="substring('-------',0,count(ancestor::*))"/> Word <xsl:value-of select="@address"/></td><td><xsl:value-of select="@id"/></td>
        <td><xsl:value-of select="@mask"/></td><td align="center"><xsl:value-of select="@permission"/></td><td><xsl:value-of select="@description"/></td>
        </tr>
</xsl:template>

<xsl:template match="node[node and not(node/@address)]" priority="3"><!-- Bit table header (nodes with children that have no address)-->
        <tr>
	<xsl:choose>
		<xsl:when test="parent::node[@address]">
			<td><xsl:value-of select="substring('-------',0,count(ancestor::*))"/> word <xsl:value-of select="@address"/></td>
                        <td><xsl:value-of select="@id"/></td><td><xsl:value-of select="@mask"/></td>
                        <td align="center"><xsl:value-of select="@permission"/></td><td><xsl:value-of select="@description"/></td>
		</xsl:when>
		<xsl:otherwise><!-- table on first level -->
			<td><strong><xsl:value-of select="@address"/></strong></td><td><strong><xsl:value-of select="@id"/></strong></td>
                        <td><strong><xsl:value-of select="@mask"/></strong></td><td align="center"><xsl:value-of select="@permission"/></td>
                        <td><xsl:value-of select="@description"/></td>
		</xsl:otherwise>			
	</xsl:choose>
        </tr>
        <tr><td> </td>
        <td colspan="3">
	<table border="1">	
	<tr><th>Mask</th><th>Bits</th><th>Name</th><th>Description</th></tr>
		<xsl:apply-templates/>
	</table>
        </td></tr>
</xsl:template>

<xsl:template match="node[not(@address) and ancestor::node]"><!-- Bit table row (node without address and ancestor)-->
<tr><td><xsl:value-of select="@mask"/></td>
<td align="center">[<xsl:variable name="begin">
	<xsl:call-template name="last_bit"><xsl:with-param name="mask" select="concat(substring(@mask,3),'x')"/></xsl:call-template>
</xsl:variable>
<xsl:variable name="end">
	<xsl:call-template name="first_bit"><xsl:with-param name="mask" select="@mask"/></xsl:call-template>
</xsl:variable>
<xsl:choose>
	<xsl:when test="$begin = $end"><!-- Only one bit -->
		<xsl:value-of select="$begin"/>
	</xsl:when>
	<xsl:otherwise><!-- Several bits -->
		<xsl:value-of select="concat($begin, '-', $end)"/>
	</xsl:otherwise>
</xsl:choose>]</td>
<td><xsl:value-of select="@id"/></td>
<td><xsl:value-of select="@description"/></td> 
</tr>
</xsl:template>

<!-- Recursive template to calculate the last (most significant) set bit of a mask -->
<xsl:template name="last_bit">
	<xsl:param name="mask"/>
	<xsl:variable name="first" select="substring($mask,1,1)"/>
	<xsl:choose>
		<xsl:when test="$first = '0'">
			<xsl:variable name="recursive_result">
			        <xsl:call-template name="last_bit">
					<xsl:with-param name="mask" select="substring($mask, 2, string-length($mask))"/>
				</xsl:call-template>
			</xsl:variable>
			<xsl:value-of select="$recursive_result - 4"/>
		</xsl:when>
		<xsl:when test="$first='1'">
			<xsl:value-of select="28"/>
		</xsl:when>
		<xsl:when test="$first='2' or $first='3'">
			<xsl:value-of select="29"/>
		</xsl:when>
		<xsl:when test="$first='4' or $first='5' or $first='6' or $first='7'">
			<xsl:value-of select="30"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="31"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- Recursive template to calculate the first (least significant) set bit of a mask -->
<xsl:template name="first_bit">
	<xsl:param name="mask"/>
	<xsl:variable name="last" select="substring($mask,string-length($mask),string-length($mask))"/>
	<xsl:choose>
		<xsl:when test="$last = '0'">
			<xsl:variable name="recursive_result">
			        <xsl:call-template name="first_bit">
					<xsl:with-param name="mask" select="substring($mask, 1, string-length($mask)-1)"/>
				</xsl:call-template>
			</xsl:variable>
			<xsl:value-of select="$recursive_result + 4"/>
		</xsl:when>
		<xsl:when test="$last='8'">
			<xsl:value-of select="3"/>
		</xsl:when>
		<xsl:when test="$last='4' or $last='C' or $last='c'">
			<xsl:value-of select="2"/>
		</xsl:when>
		<xsl:when test="$last='2' or $last='6' or $last='A' or $last='E' or $last='a' or $last='e'">
			<xsl:value-of select="1"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="0"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>
</xsl:stylesheet>


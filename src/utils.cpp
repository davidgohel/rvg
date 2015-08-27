/*
 * This file is part of ReporteRs.
 * Copyright (c) 2014, David Gohel All rights reserved.
 *
 * ReporteRs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ReporteRs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ReporteRs.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "utils.h"
#include "datastruct.h"


void set_tracer_on(int *dn) {
	pGEDevDesc dev= GEgetDevice(*dn);
	if (dev) {
		DOCDesc *pd = (DOCDesc *) dev->dev->deviceSpecific;
		pd->elt_tracer->on = 1;
		pd->elt_tracer->isinit = 0;
		pd->elt_tracer->first_elt = -1;
		pd->elt_tracer->last_elt = -1;
	}
}


void set_tracer_off(int *dn) {
	pGEDevDesc dev= GEgetDevice(*dn);
	if (dev) {
		DOCDesc *pd = (DOCDesc *) dev->dev->deviceSpecific;
		pd->elt_tracer->on = 0;
		pd->elt_tracer->isinit = 0;
		pd->elt_tracer->first_elt = -1;
		pd->elt_tracer->last_elt = -1;
	}
}

void collect_id(int *dn, int *res) {
	pGEDevDesc dev= GEgetDevice(*dn);
	if (dev) {
		DOCDesc *pd = (DOCDesc *) dev->dev->deviceSpecific;

		res[0] = pd->elt_tracer->first_elt;
		res[1] = pd->elt_tracer->last_elt;
	}
}

void add_tooltip(int *dn, int *id, char **str, int *l){
	int nb_elts = *l;
	int i;
	pGEDevDesc dev= GEgetDevice(*dn);
	if (!dev) return;

	DOCDesc *pd = (DOCDesc *) dev->dev->deviceSpecific;
	fputs("<script type=\"text/javascript\"><![CDATA[", pd->dmlFilePointer);

	for( i = 0 ; i < nb_elts ; i++ ){
		fprintf(pd->dmlFilePointer, "$(\"#svg_%d\").find(\"#elt_%d\").attr(\"data-toggle\",\"tooltip\").attr(\"title\",\"%s\")",
				pd->canvas_id, id[i], str[i] );
		fputs(".attr(\"data-html\",\"true\").tooltip({'container':'body','placement':'bottom'});", pd->dmlFilePointer);
	}
	fputs("]]></script>", pd->dmlFilePointer);

}
void add_click_event(int *dn, int *id, char **str, int *l){
	int nb_elts = *l;
	int i;
	pGEDevDesc dev= GEgetDevice(*dn);
	if (!dev) return;
	DOCDesc *pd = (DOCDesc *) dev->dev->deviceSpecific;
	fputs("<script type=\"text/javascript\"><![CDATA[", pd->dmlFilePointer);
	for( i = 0 ; i < nb_elts ; i++ ){
		fprintf(pd->dmlFilePointer, "$(\"#svg_%d\").find(\"#elt_%d\").click(%s);",
				pd->canvas_id, id[i], str[i] );
	}
	fputs("]]></script>", pd->dmlFilePointer);

}
void add_data_id(int *dn, int *id, char **datid, int *l){
	int nb_elts = *l;
	int i;
	pGEDevDesc dev= GEgetDevice(*dn);
	if (!dev) return;

	DOCDesc *pd = (DOCDesc *) dev->dev->deviceSpecific;
	fputs("<script type=\"text/javascript\"><![CDATA[", pd->dmlFilePointer);
	for( i = 0 ; i < nb_elts ; i++ ){
		fprintf(pd->dmlFilePointer, "$(\"#svg_%d\").find(\"#elt_%d\").attr(\"data-id\",\"%s\");",
				pd->canvas_id, id[i], datid[i] );
	}
	fputs("]]></script>", pd->dmlFilePointer);
}

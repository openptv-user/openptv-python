/* Implementation for parameters handling routines. */

#include "parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* read_sequence_par() reads sequence parameters from a config file with the
   following format: each line is a value, first 4 values are image names,
   5th is the first number in the sequence, 6th line is the last value in the
   sequence.
   
   Arguments:
   char *filename - path to the text file containing the parameters.
   
   Returns:
   Pointer to a newly-allocated sequence_par structure. Don't forget to free
   the new memory that is allocated for the image names. If reading failed for 
   any reason, returns NULL.
*/
sequence_par* read_sequence_par(char *filename) {
    char line[SEQ_FNAME_MAX_LEN];
    FILE* par_file;
    int cam, read_ok;
    sequence_par *ret;
    
    par_file = fopen(filename, "r");
    if (par_file == NULL) {
        return NULL;
    }
    
    ret = (sequence_par *) malloc(sizeof(sequence_par));
    ret->img_base_name = (char **) calloc(4, sizeof(char *));
    
    /* Note the assumption of 4 cameras. Fixing this requires changing the
       file format. */
    for (cam = 0; cam < 4; cam++) {
        read_ok = fscanf(par_file, "%s\n", line);
        if (read_ok == 0) goto handle_error;
        
        ret->img_base_name[cam] = (char *) malloc(SEQ_FNAME_MAX_LEN);
        strncpy(ret->img_base_name[cam], line, SEQ_FNAME_MAX_LEN);
    }
    
    if( (read_ok = fscanf(par_file, "%d\n", &(ret->first))) == 0)
        goto handle_error;
    if( (read_ok = fscanf(par_file, "%d\n", &(ret->last))) == 0)    
        goto handle_error;

    fclose(par_file);
    return ret;
    
handle_error:
    printf("Error reading sequence parameters from %s\n", filename);
    free(ret);
    fclose(par_file);
    return NULL;
}

/* read_track_par() reads tracking parameters from a config file with the
   following format: each line is a value, in this order:
   1. dvxmin
   2. dvxmax
   3. dvymin
   4. dvymax
   5. dvzmin
   6. dvzmax
   7. dangle
   8. dacc
   9. add
   
   Arguments:
   char *filename - path to the text file containing the parameters.
   
   Returns:
   Pointer to a newly-allocated track_par structure. If reading failed for 
   any reason, returns NULL.
*/
track_par* read_track_par(char *filename) {
    FILE* fpp;
    track_par *ret = (track_par *) malloc(sizeof(track_par));
    
    fpp = fopen(filename, "r");
    if(fscanf(fpp, "%lf\n", &(ret->dvxmin)) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->dvxmax)) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->dvymin)) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->dvymax)) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->dvzmin)) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->dvzmax)) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->dangle)) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->dacc)) == 0) goto handle_error;
    if(fscanf(fpp, "%d\n", &(ret->add)) == 0) goto handle_error;
    fclose (fpp);
    
    ret->dsumg = ret->dn = ret->dnx = ret->dny = 0;
    return ret;

handle_error:
    free(ret);
    fclose (fpp);
    return NULL;
}

/* compare_track_par() checks that all fields of two track_par objects are
   equal.
   
   Arguments:
   track_par *t1, track_par *t2 - addresses of the objects for comparison.
   
   Returns:
   True if equal, false otherwise.
*/
int compare_track_par(track_par *t1, track_par *t2) {
    return ((t1->dvxmin == t2->dvxmin) && (t1->dvxmax == t2->dvxmax) && \
        (t1->dvymin == t2->dvymin) && (t1->dvymax == t2->dvymax) && \
        (t1->dvzmin == t2->dvzmin) && (t1->dvzmax == t2->dvzmax) && \
        (t1->dacc == t2->dacc) && (t1->dangle == t2->dangle) && \
        (t1->dsumg == t2->dsumg) && (t1->dn == t2->dn) && \
        (t1->dnx == t2->dnx) && (t1->dny == t2->dny) && (t1->add == t2->add));
}

/* read_volume_par() reads parameters of illuminated volume from a config file
   with the following format: each line is a value, in this order:
   1. X_lay[0]
   2. Zmin_lay[0]
   3. Zmax_lay[0]
   4. X_lay[1]
   5. Zmin_lay[1]
   6. Zmax_lay[1]
   
   Arguments:
   char *filename - path to the text file containing the parameters.
   
   Returns:
   Pointer to a newly-allocated volume_par structure. If reading failed for 
   any reason, returns NULL.
*/
volume_par* read_volume_par(char *filename) {
    FILE* fpp;
    volume_par *ret = (volume_par *) malloc(sizeof(volume_par));
    
    fpp = fopen(filename, "r");
    if(fscanf(fpp, "%lf\n", &(ret->X_lay[0])) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->Zmin_lay[0])) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->Zmax_lay[0])) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->X_lay[1])) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->Zmin_lay[1])) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->Zmax_lay[1])) == 0) goto handle_error;
    
    if(fscanf(fpp, "%lf\n", &(ret->cnx)) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->cny)) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->cn)) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->csumg)) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->corrmin)) == 0) goto handle_error;
    if(fscanf(fpp, "%lf\n", &(ret->eps0)) == 0) goto handle_error;
    
    fclose (fpp);
    return ret;

handle_error:
    free(ret);
    fclose(fpp);
    return NULL;
}

/* compare_volume_par() checks that all fields of two volume_par objects are
   equal.
   
   Arguments:
   volume_par *v1, volume_par *v2 - addresses of the objects for comparison.
   
   Returns:
   True if equal, false otherwise.
*/
int compare_volume_par(volume_par *v1, volume_par *v2) {
    return ( 
        (v1->X_lay[0] == v2->X_lay[0]) && \
        (v1->Zmin_lay[0] == v2->Zmin_lay[0]) && \
        (v1->Zmax_lay[0] == v2->Zmax_lay[0]) && \
        (v1->X_lay[1] == v2->X_lay[1]) && \
        (v1->Zmin_lay[1] == v2->Zmin_lay[1]) && \
        (v1->Zmax_lay[1] == v2->Zmax_lay[1]) &&
        (v1->cn == v2->cn) && (v1->cnx == v2->cnx) && \
        (v1->cny == v2->cny) && (v1->csumg == v2->csumg) && \
        (v1->corrmin == v2->corrmin) && (v1->eps0 == v2->eps0) );
}

/*  read_control_par() reads general control parameters that are not present in
    other config files but are needed generally. The arguments are read in
    this order:
    
    1. num_cams - number of cameras in a frame.
    2n (n = 1..num_cams). img_base_name
    2n + 1. cal_img_base_name
    
    Arguments:
    char *filename - path to the text file containing the parameters.
    
    Returns:
    Pointer to a newly-allocated control_par structure. If reading failed for 
    any reason, returns NULL.
*/
control_par* read_control_par(char *filename) {
    char line[SEQ_FNAME_MAX_LEN];
    FILE* par_file;
    int cam;
    control_par *ret = (control_par *) malloc(sizeof(control_par));
    
    par_file = fopen(filename, "r");
    if(fscanf(par_file, "%d\n", &(ret->num_cams)) == 0) goto handle_error;
    
    ret->img_base_name = (char **) calloc(ret->num_cams, sizeof(char*));
    ret->cal_img_base_name = (char **) calloc(ret->num_cams, sizeof(char *));
    
    for (cam = 0; cam < ret->num_cams; cam++) {
        if (fscanf(par_file, "%s\n", line) == 0) goto handle_error;
        ret->img_base_name[cam] = (char *) malloc(SEQ_FNAME_MAX_LEN);
        strncpy(ret->img_base_name[cam], line, SEQ_FNAME_MAX_LEN);
        
        if (fscanf(par_file, "%s\n", line) == 0) goto handle_error;
        ret->cal_img_base_name[cam] = (char *) malloc(SEQ_FNAME_MAX_LEN);
        strncpy(ret->cal_img_base_name[cam], line, SEQ_FNAME_MAX_LEN);
    }
    
    return ret;
    fclose(par_file);

handle_error:
    fclose(par_file);
    free_control_par(ret);    
    return NULL;
}

/*  free_control_par() frees a control_par pointer and the memory allocated
    under it fro image namew etc.
    
    Arguments:
    control_par *cp - pointer to the control_par object to destroy.
*/
void free_control_par(control_par *cp) {
    int cam;
    
    for (cam = 0; cam < cp->num_cams; cam++) {
        if (cp->img_base_name[cam] == NULL) break;
        free(cp->img_base_name[cam]);
        
        if (cp->cal_img_base_name[cam] == NULL) break;
        free(cp->cal_img_base_name[cam]);
    }
    free(cp);
}

/* compare_control_par() checks that two control_par objects are deeply-equal,
   i.e. the memorry allocations contain equal values, and other fields are
   directly equal.
   
   Arguments:
   control_par *v1, *v2 - addresses of the objects for comparison.
   
   Returns:
   True if equal, false otherwise.
*/
int compare_control_par(control_par *c1, control_par *c2) {
    int cam;
    
    if (c1->num_cams != c2->num_cams) return 0;
    
    for (cam = 0; cam < c1->num_cams; cam++) {
        if (strncmp(c1->img_base_name[cam], c2->img_base_name[cam],
            SEQ_FNAME_MAX_LEN - 1) != 0) return 0;
        if (strncmp(c1->cal_img_base_name[cam], c2->cal_img_base_name[cam],
            SEQ_FNAME_MAX_LEN - 1) != 0) return 0;
    }
    return 1;
}


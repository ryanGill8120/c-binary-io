
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE_NAME "courses.dat"


typedef struct 
{
    char course_Name[80];
    char course_Sched[4];
    unsigned course_Hours;
    unsigned course_Size;
    unsigned padding;
} COURSE;



int main(int argc, char *argv[]){

    FILE *dataFile;
    
    COURSE course;
    char sizeInputBuffer[101];
    char courseName[80];
    char courseSchedule[4];
    char chosenOption;
    char chosenCourseNumString[10];
    char chosenCreditHoursString[10];
    char chosenEnrollmentString[10];
    int chosenCourseNum;
    unsigned chosenCreditHours;
    unsigned chosenEnrollment;
    int scanfCount;
    int seekSuccess;
    long readSuccess;
    long writeSuccess;
    long RBA;
    long courseNumber;

    

    //opens for read binary plus first so that existing data is not overwritten
    dataFile = fopen(DATA_FILE_NAME, "rb+");

    //if no file was detected, create one
    if (dataFile == NULL) {
        dataFile = fopen(DATA_FILE_NAME, "wb+");
    }
    

    printf("\nEnter one of the following options or press CTRL-D to exit.\n");
    printf("C - create a new course record\n");
    printf("R - read an existing course record\n");
    printf("U - update an existing course record\n");
    printf("D - delete an existing course record\n\n");

    while(fgets(sizeInputBuffer, 100, stdin) != NULL){

        //checks for 'empty line', enter pressed on linux
        if (sizeInputBuffer[0] == '\n')
            continue;
        
        scanfCount = sscanf(sizeInputBuffer, "%c", &chosenOption);

        //invalid input
        if (scanfCount != 1){
            printf("ERROR: Invalid input\n");
            continue;
        }
        switch(chosenOption){

            //CREATE block
            case 'C':
            case 'c':
                printf("Create a new course, enter one line at a time:\nCourse Number:\n");
                while(fgets(sizeInputBuffer, 100, stdin) != NULL){

                    //allows enter-strokes/whitespace
                    if (sizeInputBuffer[0] == '\n')
                        continue;
                    scanfCount = sscanf(sizeInputBuffer, "%s", chosenCourseNumString);
                    chosenCourseNum = atoi(chosenCourseNumString);

                    //int conversion was successful, calculate random byte offset
                    if (chosenCourseNum > 0){
                        courseNumber = (long) chosenCourseNum;
                        RBA = courseNumber * sizeof(COURSE);
                        break;
                    }else{
                        printf("Invalid course number Input\n");
                        continue;
                    }
                }
                
                //call to fseek with calculated RBA, return value in seekSuccess
                seekSuccess = fseek(dataFile, RBA, SEEK_SET);

                //checks that fseek was successful and that the course info is valid input. Here, a deleted or empty lesser
                //value for the course number could result in a valid search, so we also check for valid values within struct 
                if (seekSuccess == 0){

                    //course_Sched strings are error checked on entry, they will only be one of these two values
                    if (strcmp(course.course_Sched, "TR") != 0 && strcmp(course.course_Sched, "MWF") != 0){
                    
                        //begin input loops
                        printf("Course Name:\n");
                        while(fgets(sizeInputBuffer, 100, stdin) != NULL){

                            //allows enter-strokes/whitespace
                            if (sizeInputBuffer[0] == '\n')
                                continue;
                            
                            //explicitly clears struct data and then
                            memset(course.course_Name, '\0', sizeof(course.course_Name));
                            strncpy(course.course_Name, sizeInputBuffer, 80);
                            break;
                        }

                        //near identical input looping for rest of values in both create and update blocks

                        printf("Course Schedule:\n");
                        while(fgets(sizeInputBuffer, 100, stdin) != NULL){

                            //allows enter-strokes/whitespace
                            if (sizeInputBuffer[0] == '\n')
                                continue;
                            scanfCount = sscanf(sizeInputBuffer, "%s", courseSchedule);
                            if (strcmp(courseSchedule, "TR") == 0 || strcmp(courseSchedule, "MWF") == 0){
                                memset(course.course_Sched, '\0', sizeof(course.course_Sched));
                                strncpy(course.course_Sched, courseSchedule, 4);
                            }else{
                                printf("Invalid Schedule input\n");
                                continue;
                            }
                            break;
                        }
                        printf("Course Credit Hours:\n");
                        while(fgets(sizeInputBuffer, 100, stdin) != NULL){

                            //allows enter-strokes/whitespace
                            if (sizeInputBuffer[0] == '\n')
                                continue;
                            scanfCount = sscanf(sizeInputBuffer, "%s", chosenCreditHoursString);
                            chosenCreditHours = (unsigned) atoi(chosenCreditHoursString); 
                            course.course_Hours = chosenCreditHours;
                            break;
                        }
                        printf("Course Enrollment:\n");
                        while(fgets(sizeInputBuffer, 100, stdin) != NULL){

                            //allows enter-strokes/whitespace
                            if (sizeInputBuffer[0] == '\n')
                                continue;
                            scanfCount = sscanf(sizeInputBuffer, "%s", chosenEnrollmentString);
                            chosenEnrollment = (unsigned) atoi(chosenEnrollmentString); 
                            course.course_Size = chosenEnrollment;
                            break;
                        }
                        writeSuccess = fwrite(&course, sizeof(COURSE), 1L, dataFile);
                        if (writeSuccess != 1)
                            printf("write failure\n");
                    }else{
                        printf("ERROR (seek success): course already exists\n");
                    }
                }
                break;

            //READ block
            case 'R':
            case 'r':
                printf("Enter a course number:\n");
                while(fgets(sizeInputBuffer, 100, stdin) != NULL){

                    //allows enter-strokes/whitespace
                    if (sizeInputBuffer[0] == '\n')
                        continue;

                    //gets course num input and converts to int
                    scanfCount = sscanf(sizeInputBuffer, "%s", chosenCourseNumString);
                    chosenCourseNum = atoi(chosenCourseNumString);

                    //checks if string input was valid, then calculates RBA and searches directly
                    if (chosenCourseNum > 0){
                        courseNumber = (long) chosenCourseNum;
                        RBA = courseNumber * sizeof(COURSE);

                    
                        seekSuccess = fseek(dataFile, RBA, SEEK_SET);

                        //checks for successful search and read, then prints output to user
                        if (seekSuccess == 0){
                            readSuccess = fread(&course, sizeof(COURSE), 1L, dataFile);
                            if (readSuccess == 1){

                                //again, looks at actual values in struct to determine valid/invalid record
                                if (strcmp(course.course_Sched, "TR") != 0 && strcmp(course.course_Sched, "MWF") != 0){
                                    printf("ERROR: course not found\n");
                                }else{
                                    printf("\nCourse number: %d\n", chosenCourseNum);
                                    printf("Course name: %s", course.course_Name);
                                    printf("Scheduled Days: %s\n", course.course_Sched);
                                    printf("Credit Hours: %u\n", course.course_Hours);
                                    printf("Enrolled Students: %u\n", course.course_Size);
                                }
                            //multiple points of possible failure, hence seemingly redundant error messages    
                            }else{
                                printf("ERROR: course not found\n");
                            }
                            break;
                        }else{
                           printf("ERROR: course not found\n");
                           break; 
                        }
                    }else{
                        printf("ERROR: course not found\n");
                        break;
                    }
                }
                break;

            //UPDATE block
            //functionality here is to first determine if the course exists, if it does, then perform a read to capture data into
            //the active struct so that if no changes are made there is a point of reference, then write changed struct
            case 'U':
            case 'u':
                printf("Enter a course number:\n");
                while(fgets(sizeInputBuffer, 100, stdin) != NULL){

                    //allows enter-strokes/whitespace
                    if (sizeInputBuffer[0] == '\n')
                        continue;

                    //gets course num input and converts to int
                    scanfCount = sscanf(sizeInputBuffer, "%s", chosenCourseNumString);
                    chosenCourseNum = atoi(chosenCourseNumString);

                    //checks if string input was valid, then calculates RBA and searches directly
                    if (chosenCourseNum > 0){
                        courseNumber = (long) chosenCourseNum;
                        RBA = courseNumber * sizeof(COURSE);
                        seekSuccess = fseek(dataFile, RBA, SEEK_SET);

                        
                        //checks for successful search and read, then loads data into courses struct (for non-changes)
                        if (seekSuccess == 0){
                            readSuccess = fread(&course, sizeof(COURSE), 1L, dataFile);
                            if (readSuccess == 1){
                                if (strcmp(course.course_Sched, "TR") != 0 && strcmp(course.course_Sched, "MWF") != 0){
                                    printf("ERROR (values): course not found\n");
                                    break;

                                //successful search and read, begin input
                                }else{

                                    //resets cursor to proper position
                                    seekSuccess = fseek(dataFile, RBA, SEEK_SET);
                                    if (seekSuccess ==0){
                                        
                                        //for all inputs in update, an enter stroke (empty line of input) will break the inividual
                                        //input loop, and the original struct in memory will remain unchanged
                                        printf("Course Name:\n");
                                        while(fgets(sizeInputBuffer, 100, stdin) != NULL){

                                            //allows enter-strokes/whitespace
                                            if (sizeInputBuffer[0] == '\n')
                                                break;
                                            
                                            //if flow reached here, the data will be input into active struct
                                            memset(course.course_Name, '\0', sizeof(course.course_Name));
                                            strncpy(course.course_Name, sizeInputBuffer, 80);
                                            break;
                                        }
                                        printf("Course Schedule:\n");
                                        while(fgets(sizeInputBuffer, 100, stdin) != NULL){

                                            //allows enter-strokes/whitespace
                                            if (sizeInputBuffer[0] == '\n')
                                                break;
                                            scanfCount = sscanf(sizeInputBuffer, "%s", courseSchedule);
                                            if (strcmp(courseSchedule, "TR") == 0 || strcmp(courseSchedule, "MWF") == 0){
                                                memset(course.course_Sched, '\0', sizeof(course.course_Sched));
                                                strncpy(course.course_Sched, courseSchedule, 4);
                                            }else{
                                                printf("Invalid Schedule input\n");
                                                continue;
                                            }
                                            break;
                                        }
                                        printf("Course Credit Hours:\n");
                                        while(fgets(sizeInputBuffer, 100, stdin) != NULL){

                                            //allows enter-strokes/whitespace
                                            if (sizeInputBuffer[0] == '\n')
                                                break;
                                            scanfCount = sscanf(sizeInputBuffer, "%s", chosenCreditHoursString);
                                            chosenCreditHours = (unsigned) atoi(chosenCreditHoursString); 
                                            course.course_Hours = chosenCreditHours;
                                            break;
                                        }
                                        printf("Course Enrollment:\n");
                                        while(fgets(sizeInputBuffer, 100, stdin) != NULL){

                                            //allows enter-strokes/whitespace
                                            if (sizeInputBuffer[0] == '\n')
                                                break;
                                            scanfCount = sscanf(sizeInputBuffer, "%s", chosenEnrollmentString);
                                            chosenEnrollment = (unsigned) atoi(chosenEnrollmentString); 
                                            course.course_Size = chosenEnrollment;
                                            break;
                                        }

                                        //write struct
                                        writeSuccess = fwrite(&course, sizeof(COURSE), 1L, dataFile);
                                        if (writeSuccess != 1)
                                            printf("write failure\n");

                                    }
                                }
                                
                            }else{
                                printf("ERROR (read): course not found\n");
                            }
                            break;
                        }else{
                           printf("ERROR (seek): course not found\n");
                           break; 
                        }
                    }else{
                        printf("ERROR (course num): course not found\n");
                        break;
                    }
                }
                break;

            //DELETE block
            case 'D':
            case 'd':
                printf("Enter a course number:\n");
                while(fgets(sizeInputBuffer, 100, stdin) != NULL){

                    //allows enter-strokes/whitespace
                    if (sizeInputBuffer[0] == '\n')
                        continue;
                    scanfCount = sscanf(sizeInputBuffer, "%s", chosenCourseNumString);
                    chosenCourseNum = atoi(chosenCourseNumString);
                    if (chosenCourseNum > 0){
                        courseNumber = (long) chosenCourseNum;
                        RBA = courseNumber * sizeof(COURSE);
                        break;
                    }else{
                        printf("Invalid course number Input\n");
                        continue;
                    }
                }
                
                seekSuccess = fseek(dataFile, RBA, SEEK_SET);
                if (seekSuccess == 0){
                    if (strcmp(course.course_Sched, "TR") == 0 || strcmp(course.course_Sched, "MWF") == 0){

                        //after successful search of course, the deletion will occur by returning the bytes to the state
                        //they would be if they were padded by a larger index if write, that is, all zeroes/empty strings
                    
                        memset(course.course_Name, '\0', sizeof(course.course_Name));
                        strcpy(course.course_Name, "");

                        memset(course.course_Sched, '\0', sizeof(course.course_Sched));
                        strcpy(course.course_Sched, "");

                        course.course_Hours = 0;

                        course.course_Size = 0;

                        writeSuccess = fwrite(&course, sizeof(COURSE), 1L, dataFile);
                        if (writeSuccess != 1)
                            printf("delete failure\n");
                        
                    }else{
                        printf("ERROR: course not found\n");
                    }
                }else{
                    printf("ERROR: course not found\n");
                }
                break;
            default:
                printf("ERROR: invalid option\n");
        }

        //prompt for each loop
        printf("\nEnter one of the following options or press CTRL-D to exit.\n");
        printf("C - create a new course record\n");
        printf("R - read an existing course record\n");
        printf("U - update an existing course record\n");
        printf("D - delete an existing course record\n\n");

    }

    fclose(dataFile);
    return 0;
}

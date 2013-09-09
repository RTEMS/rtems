/**
 * @file Create_Files.cs
 *
 * @brief Format a USB memory stick and create files on the stick
 * Small C# application which formats a USB memory stick with a FAT
 * file system and creates files on the memory stick. The files will
 * have names with various multibyte strings.
 * The whole procedure is part of creating a MS Windows FAT disk image
 * whith files with exotic multibyte filenames. The image will get 
 * mounted under RTEMS for testing the mutibyte/UTF-8 feature of the
 * RTEMS FAT file system and the compatibility to MS Windows.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */
using System;
using System.Collections.Generic;
using System.IO;
using System.Diagnostics;
using System.Threading;
using System.Text;

namespace Create_Files
{
    public static class Create_Files
    {
        // Strings for file names and file contents
        public static string[] Strings = new string[] { 
                "this is a long filename", 
                "đây là một tên tập tin dài",
                "Bu uzun bir dosya adı",
                "هذا هو اسم ملف طويل",
                "αυτό είναι ένα μεγάλο όνομα αρχείου",
                "это длинное имя",
                "гэта доўгае імя",
                "това е дълго име на файла",
                "这是一个长文件名",
                "মেৰিকা মহাদেশ, উত্তৰ আমেৰিকা আৰু দক্ষিণ আমেৰিকা মহাদেশক লৈ গঠিত এক",
                "آمریکا قاره یکته قارهٰ زمینˇ قاره‌ٰنˇ مئن ایسسه کی زمینˇ هنه‌شر (مساحت)ˇ جی ۳۸٪ و زمینˇ خوشکی‌ئنˇ جی ۴۲۸٪ ای قاره شی ایسسه", 
                "Manâhestôtse 910,720,588 (July 2008 est.)",
                "Elle s'étend depuis l'océan Arctique au nord jusqu'au cap Horn dans le passage de Drake au sud, à la confluence des océans", 
                "ཨ་མེ་རི་ཀ, ཨ་མེ་རི་ཁ, མེ་གླིང་", 
                "е су земље западне хемисфере или Новог света које се састоје од континената Северна Америка", 
                "This is a filename with with 255 characters. The following numbers are aligned in that way, that the character 0 is the mentioned one. xx140xxxxxxx150xxxxxxx160xxxxxxx170xxxxxxx180xxxxxxx190xxxxxxx200xxxxxxx210xxxxxxx220xxxxxxx230xxxxxxx240xxxxxxx250xxxxx", 
                "Bu gezegen Roma mitolojisindeki savaş ilahı Mars'a", 
                "Amerike su zemlje zapadne hemisfere ili Novog svijeta koje se sastoje od kontinenata Sjeverna Amerika i Južna Amerika sa svim pridruženim otocima i regijama.", 
                "იანებს ორ კონტინენტს, სამხრეთ და ჩრდილოეთ ამერიკას ახლომდებარე კუნძულებთან ერ",
                " Є то єдиный контінент, котрого цїла теріторія лежыть на Западній півкулї тай разом"
        };

        // Use the features of MS Windows to format the USB memory stick. We want a genuine Microsoft FAT file system
        public static void FormatDrive(string driveLetter)
        {
            ProcessStartInfo StartInfo = new ProcessStartInfo();
            StartInfo.FileName = Environment.SystemDirectory + "\\cmd.exe";
            StartInfo.Arguments = "/C \"format " + driveLetter + " /FS:FAT\"";
            StartInfo.UseShellExecute = false;
            StartInfo.RedirectStandardInput = true;
            Process Process = Process.Start(StartInfo);
            //Thread.Sleep(1000);
            Process.StandardInput.WriteLine();
            Process.StandardInput.WriteLine();
            Process.WaitForExit();
        }
        // Format a USB meory stick and create files on the new volume
        // args[0] The drive to be formatted. E.g. "e:"
        public static void Main(string[] args)
        {
            // Display help text on the console
            if ((args.Length <= 0) || (args[0].Equals("-h", StringComparison.InvariantCultureIgnoreCase) || args[0].Equals("-help", StringComparison.InvariantCultureIgnoreCase)))
            {
                Console.WriteLine("create_files.bat <DRIVE>");
                Console.WriteLine("Will format DRIVE and create files on the new formated drive.");
            }
            else
            {
                // Show a warning
                Console.WriteLine(args[0] + " will get formated!");
                while (true)
                {
                    Console.WriteLine("Press y to continue or press n to abort. [y\\n]");
                    string Input = Console.ReadLine();
                    if (Input[0].Equals('y') || Input[0].Equals('Y'))
                        break;
                    else if (Input[0].Equals('n') || Input[0].Equals('N'))
                        return;
                }
                // Format the USB memory stick
                FormatDrive(args[0]);

                // Create the files and write their own file names into them
                for (int i = 0; i < Strings.GetLength(0); i++)
                {
                    File.WriteAllText(Path.Combine(args[0], Strings[i]), Strings[i], Encoding.UTF8);
                    Console.WriteLine("The file \"" + Strings[i]  + "\" created.");
                }

                // Create a c header file which contains an array with the strings and a #define 
                // for the number of strings
                string HeaderPath = Path.Combine(args[0]/*Environment.CurrentDirectory*/, "files.h");
                Console.WriteLine("The header \"" + HeaderPath + "\" will write.");
                StreamWriter HeaderStream = new StreamWriter(HeaderPath);

                HeaderStream.Write("\n" +
                                    "/*\n" +
                                    " *  Array with files, that were created in the FAT-filesystem image.bin.\n" +
                                    " *\n" +
                                    " *  WARNING: Automatically generated by Create_Files.cs -- do not edit!\n" +
                                    " */\n" +
                                    "\n" +
                                    "#ifndef __FILE_H__\n" +
                                    "#define __FILE_H__\n" +
                                    "\n" +
                                    "#ifdef __cplusplus\n" +
                                    "extern C {\n" +
                                    "#endif\n" +
                                    "\n" +
                                    "static const char *const filenames[] = {\n");
                for (int i = 0; i < Strings.GetLength(0); i++)
                    HeaderStream.WriteLine("  \"" + Strings[i] + ((i == (Strings.GetLength(0) - 1)) ? "\"" : "\","));
                string NumberOfFilesStr = Strings.GetLength(0).ToString(System.Globalization.CultureInfo.InvariantCulture);
                HeaderStream.Write("};\n" +
                                    "#define FILES_FILENAMES_NUMBER_OF " + NumberOfFilesStr + "\n" +
                                    "\n" +
                                    "#ifdef __cplusplus\n" +
                                    "}\n" +
                                    "#endif\n" +
                                    "\n" +
                                    "#endif /* __FILE_H__ */\n" +
                                    "\n");

                // Finalize
                HeaderStream.Flush();
                HeaderStream.Close();
                HeaderStream.Dispose();
            }
        }
    }
}

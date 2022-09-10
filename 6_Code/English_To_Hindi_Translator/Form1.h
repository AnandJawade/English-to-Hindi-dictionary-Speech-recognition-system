#pragma once

#include "stdAfx.h"
#include<iostream>
#include<cstdlib>
#include<ctime>
#include<stdlib.h>
#include<math.h>
#include<conio.h>
#include<string.h>
#include<vector>
#include<fstream>
#include<time.h>
#include "Form2.h"

#include "HMM.h"

#define N 5
#define M 32
#define T_MAX 150
#define P 12
#define training_utterances 20
#define testing_utterances 10
#define frame_size 320

namespace English_To_Hindi_Translator {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		

	public: 
		
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			srand(time(0));

			for(int i = 0; i < 10; ++i)
				this->comboBox1->Items->Add(gcnew String(words[i]));

			in.open("./Input/Codebook.txt");
			for(int i = 0; i < M; ++i)
			{
				for(int j = 0; j < P; ++j)
				{
					in >> codebook[i][j];
				}
			}
			in.close();

			for(int digit = 0; digit < 10; ++digit)
			{
				char model_filepath[1000] = "";
				_snprintf(model_filepath, sizeof(model_filepath), "%s%s%s", "./Output/Models/Model_", digits[digit], extension);
				//printf("Reading File %s\n", model_filepath);
				read_model(model_filepath, digit);
			}
		}

		

	protected:

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  start_recording_button;
	protected: 

	protected: 


	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  word_detected;
	private: System::Windows::Forms::Label^  meaning;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::ComboBox^  comboBox1;


	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  button1;


	private: System::Windows::Forms::NumericUpDown^  numericUpDown1;
	private: System::Windows::Forms::Label^  number_of_utterances;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::Label^  correct_answer;
	private: System::Windows::Forms::Label^  test_word_detected;


	private: System::Windows::Forms::Label^  label6;


	private: System::Windows::Forms::Button^  button4;
	private: System::Windows::Forms::Label^  random_word;

	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::PictureBox^  pictureBox1;


	protected: 

	protected: 




	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->start_recording_button = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->word_detected = (gcnew System::Windows::Forms::Label());
			this->meaning = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->number_of_utterances = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->correct_answer = (gcnew System::Windows::Forms::Label());
			this->test_word_detected = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->random_word = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->BeginInit();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// start_recording_button
			// 
			this->start_recording_button->Location = System::Drawing::Point(96, 132);
			this->start_recording_button->Name = L"start_recording_button";
			this->start_recording_button->Size = System::Drawing::Size(179, 42);
			this->start_recording_button->TabIndex = 0;
			this->start_recording_button->Text = L"Start Recording";
			this->start_recording_button->UseVisualStyleBackColor = true;
			this->start_recording_button->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(44, 51);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(132, 20);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Word Detected :";
			this->label1->Click += gcnew System::EventHandler(this, &Form1::label1_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(34, 88);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(144, 20);
			this->label2->TabIndex = 3;
			this->label2->Text = L"Meaning in Hindi :";
			this->label2->Click += gcnew System::EventHandler(this, &Form1::label2_Click);
			// 
			// word_detected
			// 
			this->word_detected->AutoSize = true;
			this->word_detected->Location = System::Drawing::Point(212, 48);
			this->word_detected->Name = L"word_detected";
			this->word_detected->Size = System::Drawing::Size(0, 20);
			this->word_detected->TabIndex = 4;
			this->word_detected->Click += gcnew System::EventHandler(this, &Form1::label3_Click);
			// 
			// meaning
			// 
			this->meaning->AutoSize = true;
			this->meaning->Location = System::Drawing::Point(212, 88);
			this->meaning->Name = L"meaning";
			this->meaning->Size = System::Drawing::Size(0, 20);
			this->meaning->TabIndex = 5;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->numericUpDown1);
			this->groupBox1->Controls->Add(this->number_of_utterances);
			this->groupBox1->Controls->Add(this->button1);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->comboBox1);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox1->Location = System::Drawing::Point(523, 271);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(402, 207);
			this->groupBox1->TabIndex = 6;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Train with New Utterances";
			// 
			// numericUpDown1
			// 
			this->numericUpDown1->Location = System::Drawing::Point(236, 86);
			this->numericUpDown1->Name = L"numericUpDown1";
			this->numericUpDown1->Size = System::Drawing::Size(120, 27);
			this->numericUpDown1->TabIndex = 6;
			// 
			// number_of_utterances
			// 
			this->number_of_utterances->AutoSize = true;
			this->number_of_utterances->Location = System::Drawing::Point(6, 91);
			this->number_of_utterances->Name = L"number_of_utterances";
			this->number_of_utterances->Size = System::Drawing::Size(184, 20);
			this->number_of_utterances->TabIndex = 5;
			this->number_of_utterances->Text = L"Number of Utterances :";
			this->number_of_utterances->Click += gcnew System::EventHandler(this, &Form1::number_of_utterances_Click);
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(115, 132);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(126, 42);
			this->button1->TabIndex = 4;
			this->button1->Text = L"Train";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click_1);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(6, 55);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(192, 20);
			this->label3->TabIndex = 1;
			this->label3->Text = L"Select the word to train :";
			// 
			// comboBox1
			// 
			this->comboBox1->DisplayMember = L"List";
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Location = System::Drawing::Point(235, 48);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(121, 28);
			this->comboBox1->TabIndex = 0;
			this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::comboBox1_SelectedIndexChanged);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->label1);
			this->groupBox2->Controls->Add(this->word_detected);
			this->groupBox2->Controls->Add(this->start_recording_button);
			this->groupBox2->Controls->Add(this->label2);
			this->groupBox2->Controls->Add(this->meaning);
			this->groupBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox2->Location = System::Drawing::Point(44, 271);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(381, 207);
			this->groupBox2->TabIndex = 7;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Translate";
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->correct_answer);
			this->groupBox3->Controls->Add(this->test_word_detected);
			this->groupBox3->Controls->Add(this->label6);
			this->groupBox3->Controls->Add(this->button4);
			this->groupBox3->Controls->Add(this->random_word);
			this->groupBox3->Controls->Add(this->label5);
			this->groupBox3->Controls->Add(this->button3);
			this->groupBox3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox3->Location = System::Drawing::Point(231, 486);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(533, 177);
			this->groupBox3->TabIndex = 8;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Test Your Knowledge";
			this->groupBox3->Enter += gcnew System::EventHandler(this, &Form1::groupBox3_Enter);
			// 
			// correct_answer
			// 
			this->correct_answer->AutoSize = true;
			this->correct_answer->Location = System::Drawing::Point(153, 140);
			this->correct_answer->Name = L"correct_answer";
			this->correct_answer->Size = System::Drawing::Size(0, 20);
			this->correct_answer->TabIndex = 6;
			// 
			// test_word_detected
			// 
			this->test_word_detected->AutoSize = true;
			this->test_word_detected->Location = System::Drawing::Point(426, 96);
			this->test_word_detected->Name = L"test_word_detected";
			this->test_word_detected->Size = System::Drawing::Size(0, 20);
			this->test_word_detected->TabIndex = 5;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(288, 96);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(132, 20);
			this->label6->TabIndex = 4;
			this->label6->Text = L"Word Detected :";
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(292, 36);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(190, 46);
			this->button4->TabIndex = 3;
			this->button4->Text = L"Start Recording";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &Form1::button4_Click);
			// 
			// random_word
			// 
			this->random_word->AutoSize = true;
			this->random_word->Location = System::Drawing::Point(122, 96);
			this->random_word->Name = L"random_word";
			this->random_word->Size = System::Drawing::Size(0, 20);
			this->random_word->TabIndex = 2;
			this->random_word->Click += gcnew System::EventHandler(this, &Form1::label6_Click);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(18, 96);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(98, 20);
			this->label5->TabIndex = 1;
			this->label5->Text = L"Your Word :";
			this->label5->Click += gcnew System::EventHandler(this, &Form1::label5_Click);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(22, 36);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(201, 46);
			this->button3->TabIndex = 0;
			this->button3->Text = L"Get a Random Word";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
			// 
			// pictureBox1
			// 
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(341, 3);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(261, 242);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBox1->TabIndex = 9;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->Click += gcnew System::EventHandler(this, &Form1::pictureBox1_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Info;
			this->ClientSize = System::Drawing::Size(1037, 694);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->EndInit();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void tabPage1_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {

				for(int digit = 0; digit < 10; ++digit)
				{
					char model_filepath[1000] = "";
					_snprintf(model_filepath, sizeof(model_filepath), "%s%s%s", "./Output/Models/Model_", digits[digit], extension);
					//printf("Reading File %s\n", model_filepath);
					read_model(model_filepath, digit);
				}

				prediction = -1;
				system("Recording_Module.exe 3 input_file.wav input_file.txt");
				process_live_data("input_file.txt");
				char text[1000];
				//text = words[prediction];
				//sprintf(text, "%d", prediction);
				if(prediction != -1)
				{
					this->word_detected->Text = gcnew String(words[prediction]);
					this->meaning->Text = gcnew String(meanings[prediction]);
				}
				else
				{
					sprintf(text, "%s", "Sorry, could not detect the word!");
					this->word_detected->Text = gcnew String(text);
					this->meaning->Text = gcnew String(text);
				}
			 }
	private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
	private: System::Void label3_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
private: System::Void label2_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->Hide();
			 Form2^ f = gcnew Form2(this);
			 f->ShowDialog();
			 //this->Close();
		 }
private: System::Void comboBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void button1_Click_1(System::Object^  sender, System::EventArgs^  e) {
			String^ s = this->comboBox1->Text;
			char word[100];
			sprintf(word, "%s", s);
			int num = Convert::ToInt32(this->numericUpDown1->Text);

			int ind = 0;
			int i, j;
			for(i = 0; i < number_of_words; ++i)
			{
				if(words[i] == word)
				{
					ind = i;
					break;
				}
			}

			for(i = 0; i < N; ++i) pi_mean[ind][i] = 0;

			for(i = 0; i < N; ++i)
				for(j = 0; j < N; ++j)
					a_mean[ind][i][j] = 0;

			for(i = 0; i < N; ++i)
				for(j = 0; j < M; ++j)
					b_mean[ind][i][j] = 0;

			initialize_HMM_globals();


			char filepath[1000] = "";
			char exec[10000] = "";
			for(i = 0; i < num; ++i)
			{
				sprintf(filepath, "%s%s%s%d%s", "./Live_data/", word, "_", i, ".txt");
				sprintf(exec, "%s%s", "Recording_Module.exe 3 input_file.wav ", filepath);

				system(exec);
				//process_live_data("input_file.txt");
				process_new_word(filepath, ind);
			}

			for(i = 0; i < N; ++i) pi_mean[ind][i] /= num;

			for(i = 0; i < N; ++i)
				for(j = 0; j < N; ++j)
					a_mean[ind][i][j] /= num;

			for(i = 0; i < N; ++i)
				for(j = 0; j < M; ++j)
					b_mean[ind][i][j] /= num;

			char model_filepath[1000] = "";
			sprintf(model_filepath, "%s%s%s%s", "./Live_data/", word, "_Model", ".txt");
			//printf("Writing into File %s\n", model_filepath);
			write_model_to_file(model_filepath, ind);

			MessageBox::Show("Training Completed!");

		 }
private: System::Void label6_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
			 random_word_ind = rand() % number_of_words;
			 random_word->Text = gcnew String(meanings[random_word_ind]);
		 }
private: System::Void button4_Click(System::Object^  sender, System::EventArgs^  e) {
				
				for(int digit = 0; digit < 10; ++digit)
				{
					char model_filepath[1000] = "";
					_snprintf(model_filepath, sizeof(model_filepath), "%s%s%s", "./Output/Models/Model_", digits[digit], extension);
					//printf("Reading File %s\n", model_filepath);
					read_model(model_filepath, digit);
				}

				prediction = -1;
				system("Recording_Module.exe 3 input_file.wav input_file.txt");
				process_live_data("input_file.txt");
				char text[1000];
				if(prediction != -1)
				{
					this->test_word_detected->Text = gcnew String(words[prediction]);
					sprintf(text, "%s%s", "Correct Answer : ", words[random_word_ind]);
					this->correct_answer->Text = gcnew String(text);
				}
				else
				{
					sprintf(text, "%s", "Sorry, could not detect the word!");
					this->test_word_detected->Text = gcnew String(text);
				}
		 }
private: System::Void number_of_utterances_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void label5_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void groupBox3_Enter(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void pictureBox1_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
};
}


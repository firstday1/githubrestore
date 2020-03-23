namespace SerialPortSENDUTF8
{
    partial class Form1
    {
        /// <summary>
        /// 設計工具所需的變數。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清除任何使用中的資源。
        /// </summary>
        /// <param name="disposing">如果應該處置受控資源則為 true，否則為 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 設計工具產生的程式碼

        /// <summary>
        /// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器修改
        /// 這個方法的內容。
        /// </summary>
        private void InitializeComponent()
        {
            this.SendButton = new System.Windows.Forms.Button();
            this.ClosePortButton = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.MessageBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.PortBox = new System.Windows.Forms.TextBox();
            this.MessageEnterButton = new System.Windows.Forms.Button();
            this.PortSetButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // SendButton
            // 
            this.SendButton.Location = new System.Drawing.Point(70, 103);
            this.SendButton.Name = "SendButton";
            this.SendButton.Size = new System.Drawing.Size(75, 23);
            this.SendButton.TabIndex = 0;
            this.SendButton.Text = "Send";
            this.SendButton.UseVisualStyleBackColor = true;
            this.SendButton.Click += new System.EventHandler(this.button1_Click);
            // 
            // ClosePortButton
            // 
            this.ClosePortButton.Location = new System.Drawing.Point(195, 103);
            this.ClosePortButton.Name = "ClosePortButton";
            this.ClosePortButton.Size = new System.Drawing.Size(75, 23);
            this.ClosePortButton.TabIndex = 1;
            this.ClosePortButton.Text = "Close";
            this.ClosePortButton.UseVisualStyleBackColor = true;
            this.ClosePortButton.Click += new System.EventHandler(this.button2_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 26);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(44, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "Message";
            // 
            // MessageBox
            // 
            this.MessageBox.Location = new System.Drawing.Point(70, 23);
            this.MessageBox.Name = "MessageBox";
            this.MessageBox.Size = new System.Drawing.Size(200, 22);
            this.MessageBox.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 67);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(24, 12);
            this.label2.TabIndex = 4;
            this.label2.Text = "Port";
            // 
            // PortBox
            // 
            this.PortBox.Location = new System.Drawing.Point(70, 65);
            this.PortBox.Name = "PortBox";
            this.PortBox.Size = new System.Drawing.Size(200, 22);
            this.PortBox.TabIndex = 5;
            // 
            // MessageEnterButton
            // 
            this.MessageEnterButton.Location = new System.Drawing.Point(276, 23);
            this.MessageEnterButton.Name = "MessageEnterButton";
            this.MessageEnterButton.Size = new System.Drawing.Size(75, 23);
            this.MessageEnterButton.TabIndex = 6;
            this.MessageEnterButton.Text = "Enter";
            this.MessageEnterButton.UseVisualStyleBackColor = true;
            this.MessageEnterButton.Click += new System.EventHandler(this.MessageEnterButton_Click);
            // 
            // PortSetButton
            // 
            this.PortSetButton.Location = new System.Drawing.Point(276, 64);
            this.PortSetButton.Name = "PortSetButton";
            this.PortSetButton.Size = new System.Drawing.Size(75, 23);
            this.PortSetButton.TabIndex = 7;
            this.PortSetButton.Text = "Enter";
            this.PortSetButton.UseVisualStyleBackColor = true;
            this.PortSetButton.Click += new System.EventHandler(this.PortSetButton_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(375, 149);
            this.Controls.Add(this.PortSetButton);
            this.Controls.Add(this.MessageEnterButton);
            this.Controls.Add(this.PortBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.MessageBox);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.ClosePortButton);
            this.Controls.Add(this.SendButton);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button SendButton;
        private System.Windows.Forms.Button ClosePortButton;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox MessageBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox PortBox;
        private System.Windows.Forms.Button MessageEnterButton;
        private System.Windows.Forms.Button PortSetButton;
    }
}

